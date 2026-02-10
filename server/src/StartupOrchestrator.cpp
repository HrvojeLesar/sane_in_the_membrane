#ifndef STARTUP_ORCHESTRATOR
#define STARTUP_ORCHESTRATOR

#include <atomic>
#include <csignal>
#include <grpcpp/resource_quota.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <mutex>
#include <optional>
#include <string>
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
            if (instance)
                instance->init_shutdown();
        }

        CStartupOrchestrator() {
            if (instance)
                return;

            instance = this;

            init_default_server_builder();

            m_threads.emplace_back(SStdinWorker(*this));
            m_threads.emplace_back(SGrpcServerWorker(*this));
            m_threads.emplace_back(SAvahiServiceWorker(*this));

            std::signal(SIGINT, sigint_handler);
            g_logger.log(TRACE, "CStartupOrchestrator constructed");
        }

        CStartupOrchestrator(CStartupOrchestrator& other)  = delete;
        CStartupOrchestrator(CStartupOrchestrator&& other) = delete;

        void wait() {
            std::lock_guard<std::mutex> lock{m_mutex};
            g_logger.log(TRACE, "CStartupOrchestrator waiting for threads");
            for (auto& thread : m_threads) {
                thread.join();
            }

            instance = nullptr;
        }

      private:
        void init_default_server_builder() {
            m_resource_quota.Resize(50 * 1024 * 1024);
            m_resource_quota.SetMaxThreads(8);

            m_builder.SetResourceQuota(m_resource_quota);
            m_builder.AddListeningPort("[::]:0", grpc::InsecureServerCredentials(), &m_port);
            m_builder.RegisterService(&m_scanner_service);

            m_grpc_server = m_builder.BuildAndStart();

            g_logger.log(INFO, std::format("gRPC bound to port: {}", m_port));
        }

        struct SStdinWorker {
            SStdinWorker(CStartupOrchestrator& orchestrator) : m_orchestrator(orchestrator) {}

            void operator()() {
                g_logger.log(INFO, "Started stdin worker");

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
                            m_orchestrator.init_shutdown();
                            break;
                        }
                    }
                }
                g_logger.log(INFO, "Shut down stdin worker");
            }

            CStartupOrchestrator& m_orchestrator;
        };

        struct SGrpcServerWorker {
            SGrpcServerWorker(CStartupOrchestrator& orchestrator) : m_orchestrator(orchestrator) {}

            void operator()() {
                g_logger.log(INFO, "Started gRPC worker");

                m_orchestrator.m_grpc_server->Wait();
                m_orchestrator.m_grpc_server.reset();

                g_logger.log(INFO, "Shut down gRPC server");
            }

            CStartupOrchestrator& m_orchestrator;
        };

        struct SAvahiServiceWorker {
            SAvahiServiceWorker(CStartupOrchestrator& orchestrator) : m_orchestrator(orchestrator) {}

            void operator()() {
                m_orchestrator.m_avahi_service.start(m_orchestrator.m_port == 0 ? std::nullopt : std::optional(m_orchestrator.m_port));

                g_logger.log(INFO, "Shut down avahi service registration");
            }

            CStartupOrchestrator& m_orchestrator;
        };

        void init_shutdown() {
            if (m_grpc_server) {
                g_logger.log(INFO, "Trying to shutdown grpc");
                m_grpc_server->Shutdown();
            }

            m_avahi_service.shutdown();
            m_read_stdin.store(false, std::memory_order::relaxed);
        }

      private:
        std::unique_ptr<grpc::Server>                      m_grpc_server{nullptr};
        std::vector<std::thread>                           m_threads{};
        mdns::CAutoRestartableAvahiService                 m_avahi_service;
        std::atomic<bool>                                  m_read_stdin{true};
        std::mutex                                         m_mutex{};

        sane_in_the_membrane::service::CScannerServiceImpl m_scanner_service{};
        grpc::ResourceQuota                                m_resource_quota{};
        grpc::ServerBuilder                                m_builder{};
        int                                                m_port{0};

        static inline CStartupOrchestrator*                instance = nullptr;
    };

}

#endif // !STARTUP_ORCHESTRATOR
