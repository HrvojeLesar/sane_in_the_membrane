#include <Sane.hpp>
#include <grpcpp/grpcpp.h>

#include "Service/ScannerService.hpp"

int main(int argc, char* argv[]) {

    service::CScannerServiceImpl was;

    grpc::ServerBuilder          builder;
    builder.AddListeningPort("localhost:50051", grpc::InsecureServerCredentials());
    builder.RegisterService(&was);
    auto server = builder.BuildAndStart();

    server->Wait();

    // if (sane::g_sane->is_ok()) {
    //     sane::g_sane->get_devices();
    // }

    return 0;
}
