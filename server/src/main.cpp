#include <Sane.hpp>
#include <grpcpp/grpcpp.h>
#include <grpcpp/resource_quota.h>
#include <sane/sane.h>

#include "Service/ScannerService.hpp"

int main(int argc, char* argv[]) {

    sane_in_the_membrane::service::CScannerServiceImpl was;

    grpc::ResourceQuota                                resource_quota{};

    resource_quota.Resize(50 * 1024);
    resource_quota.SetMaxThreads(8);

    grpc::ServerBuilder builder;
    builder.SetResourceQuota(resource_quota);
    builder.AddListeningPort("localhost:50051", grpc::InsecureServerCredentials());
    builder.RegisterService(&was);
    auto server = builder.BuildAndStart();

    server->Wait();

    return 0;
}
