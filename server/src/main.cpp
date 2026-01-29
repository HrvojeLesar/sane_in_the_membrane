#include <Sane.hpp>
#include <grpcpp/grpcpp.h>
#include <sane/sane.h>

#include "Service/ScannerService.hpp"

int main(int argc, char* argv[]) {

    service::CScannerServiceImpl was;

    grpc::ServerBuilder          builder;
    builder.AddListeningPort("localhost:50051", grpc::InsecureServerCredentials());
    builder.RegisterService(&was);
    auto server = builder.BuildAndStart();

    server->Wait();

    return 0;
}
