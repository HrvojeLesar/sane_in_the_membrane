#include <Sane.hpp>
#include <grpcpp/grpcpp.h>

#include "Connection/ServerSocket.hpp"
#include "Service/HelloWorldService.hpp"

int main(int argc, char* argv[]) {

    HelloWorldServiceImpl was;

    grpc::ServerBuilder   builder;
    builder.AddListeningPort("localhost:50051", grpc::InsecureServerCredentials());
    builder.RegisterService(&was);
    auto server = builder.BuildAndStart();

    server->Wait();

    // sane::CSane sane{};
    // if (sane.is_ok()) {
    //     sane.get_devices();
    // }

    return 0;
}
