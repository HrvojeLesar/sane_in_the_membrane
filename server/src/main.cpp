#include <Sane.hpp>
#include <grpcpp/grpcpp.h>
#include <grpcpp/resource_quota.h>
#include <iostream>
#include <sane/sane.h>
#include <string>
#include <thread>
#include "Service/ScannerService.hpp"
#include "GlobalLogger.cpp"

void start_stdin_stream(grpc::Server* server) {
    std::string input;
    while (true) {
        std::cin >> input;

        if (input == "stop" || input == "s") {
            server->Shutdown();
            break;
        }
    }
}

int main(int argc, char* argv[]) {

    sane_in_the_membrane::service::CScannerServiceImpl scanner_service;

    grpc::ResourceQuota                                resource_quota{};

    resource_quota.Resize(50 * 1024 * 1024);
    resource_quota.SetMaxThreads(8);

    grpc::ServerBuilder builder;
    builder.SetResourceQuota(resource_quota);
    builder.AddListeningPort("localhost:50051", grpc::InsecureServerCredentials());
    builder.RegisterService(&scanner_service);
    auto         server = builder.BuildAndStart();

    std::jthread in_thread{start_stdin_stream, server.get()};

    server->Wait();

    g_logger->log(DEBUG, "Shutting down");

    return 0;
}
