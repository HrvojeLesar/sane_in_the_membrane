#ifndef STARTUP_ORCHESTRATOR
#define STARTUP_ORCHESTRATOR

#include <Sane.hpp>
#include <grpcpp/grpcpp.h>
#include <grpcpp/resource_quota.h>
#include <grpcpp/server.h>
#include <iostream>
#include <memory>
#include <sane/sane.h>
#include <string>
#include <thread>
#include <vector>
#include "Service/ScannerService.hpp"
#include "GlobalLogger.cpp"
#include "mDns/AvahiServiceRegistration.hpp"

namespace sane_in_the_membrane::startup {
    class CStartupOrchestrator {
      public:
        CStartupOrchestrator() {
            m_threads.emplace_back(SStdinWorker(*this));
            m_threads.emplace_back(SGrpcServerWorker(*this));
            m_threads.emplace_back(SAvahiServiceWorker(*this));
        }

        CStartupOrchestrator(CStartupOrchestrator& other)  = delete;
        CStartupOrchestrator(CStartupOrchestrator&& other) = delete;

        ~CStartupOrchestrator() {
            for (auto& thread : m_threads) {
                thread.join();
            }
        }

      private:
        struct SStdinWorker {
            SStdinWorker(CStartupOrchestrator& orchestrator) : m_orchestrator(orchestrator) {}

            void operator()() {
                g_logger->log(INFO, "Started stdin worker");

                std::string input;
                while (true) {
                    std::cin >> input;

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

                g_logger->log(DEBUG, "Shutting down gRPC server");
            }

            CStartupOrchestrator& m_orchestrator;
        };

        struct SAvahiServiceWorker {
            SAvahiServiceWorker(CStartupOrchestrator& orchestrator) : m_orchestrator(orchestrator) {}

            void operator()() {
                m_orchestrator.m_avahi_service.start();
            }

            CStartupOrchestrator& m_orchestrator;
        };

      private:
        std::unique_ptr<grpc::Server>      m_grpc_server{nullptr};
        std::vector<std::thread>           m_threads{};
        mdns::CAutoRestartableAvahiService m_avahi_service;
    };

}

#endif // !STARTUP_ORCHESTRATOR
