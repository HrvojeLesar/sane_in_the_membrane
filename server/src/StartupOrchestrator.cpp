#ifndef STARTUP_ORCHESTRATOR
#define STARTUP_ORCHESTRATOR

#include <Sane.hpp>
#include <atomic>
#include <csignal>
#include <grpcpp/grpcpp.h>
#include <grpcpp/resource_quota.h>
#include <grpcpp/server.h>
#include <iostream>
#include <memory>
#include <sane/sane.h>
#include <string>
#include <sys/poll.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include "Service/ScannerService.hpp"
#include "GlobalLogger.cpp"
#include "mDns/AvahiServiceRegistration.hpp"

namespace sane_in_the_membrane::startup {
    class CStartupOrchestrator {
      public:
        static void sigint_handler(int) {
            if (instance->m_grpc_server)
                instance->m_grpc_server->Shutdown();

            instance->m_avahi_service.shutdown();
            instance->m_read_stdin.store(false, std::memory_order::relaxed);
        }

        CStartupOrchestrator() {
            if (instance)
                return;

            instance = this;

            m_threads.emplace_back(SStdinWorker(*this));
            m_threads.emplace_back(SGrpcServerWorker(*this));
            m_threads.emplace_back(SAvahiServiceWorker(*this));

            std::signal(SIGINT, sigint_handler);
            g_logger->log(TRACE, "CStartupOrchestrator constructed");
        }

        CStartupOrchestrator(CStartupOrchestrator& other)  = delete;
        CStartupOrchestrator(CStartupOrchestrator&& other) = delete;

        ~CStartupOrchestrator() {
            g_logger->log(TRACE, "CStartupOrchestrator waiting for threads");
            for (auto& thread : m_threads) {
                thread.join();
            }

            instance = nullptr;
            g_logger->log(TRACE, "CStartupOrchestrator destoryed");
        }

      private:
        struct SStdinWorker {
            SStdinWorker(CStartupOrchestrator& orchestrator) : m_orchestrator(orchestrator) {}

            void operator()() {
                g_logger->log(INFO, "Started stdin worker");

                pollfd pfd{
                    .fd     = STDIN_FILENO,
                    .events = POLLIN,
                };

                std::string input;
                while (m_orchestrator.m_read_stdin.load(std::memory_order::relaxed)) {

                    auto poll_result = poll(&pfd, 1, 500);

                    if (poll_result <= 0)
                        continue;

                    if (pfd.revents & POLLIN) {
                        if (!std::getline(std::cin, input))
                            break;

                        if (input == "stop" || input == "s") {
                            if (m_orchestrator.m_grpc_server) {
                                g_logger->log(INFO, "Stopping grpc");
                                m_orchestrator.m_grpc_server->Shutdown();
                            }

                            g_logger->log(INFO, "Stopping avahi");
                            m_orchestrator.m_avahi_service.shutdown();

                            break;
                        }
                    }
                }
                g_logger->log(INFO, "Shut down stdin worker");
            }

            CStartupOrchestrator& m_orchestrator;
        };

        struct SGrpcServerWorker {
            SGrpcServerWorker(CStartupOrchestrator& orchestrator) : m_orchestrator(orchestrator) {}

            void operator()() {
                g_logger->log(INFO, "Started gRPC worker");
                sane_in_the_membrane::service::CScannerServiceImpl scanner_service;

                grpc::ResourceQuota                                resource_quota{};

                resource_quota.Resize(50 * 1024 * 1024);
                resource_quota.SetMaxThreads(8);

                grpc::ServerBuilder builder;
                builder.SetResourceQuota(resource_quota);
                builder.AddListeningPort("localhost:50051", grpc::InsecureServerCredentials());
                builder.RegisterService(&scanner_service);
                m_orchestrator.m_grpc_server = builder.BuildAndStart();

                g_logger->log(INFO, "Starting gRPC server");

                m_orchestrator.m_grpc_server->Wait();

                g_logger->log(INFO, "Shut down gRPC server");
            }

            CStartupOrchestrator& m_orchestrator;
        };

        struct SAvahiServiceWorker {
            SAvahiServiceWorker(CStartupOrchestrator& orchestrator) : m_orchestrator(orchestrator) {}

            void operator()() {
                m_orchestrator.m_avahi_service.start();

                g_logger->log(INFO, "Shut down avahi service registration");
            }

            CStartupOrchestrator& m_orchestrator;
        };

      private:
        std::unique_ptr<grpc::Server>       m_grpc_server{nullptr};
        std::vector<std::thread>            m_threads{};
        mdns::CAutoRestartableAvahiService  m_avahi_service;
        std::atomic<bool>                   m_read_stdin{true};

        static inline CStartupOrchestrator* instance = nullptr;
    };

}

#endif // !STARTUP_ORCHESTRATOR
