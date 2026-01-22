#include "helloworld/v1/helloworld.grpc.pb.h"
#include "helloworld/v1/helloworld.pb.h"
#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include <iostream>

using namespace helloworld::v1;

class HelloWorldServiceImpl : public GreeterService::Service {
  public:
    grpc::Status SayHello(grpc::ServerContext* context, const SayHelloRequest* request, SayHelloResponse* response) override {
        std::cout << "Hello " << request->name() << "\n";
        response->set_message("Hello world");

        return grpc::Status::OK;
    }

    grpc::Status SayHelloStreamReply(grpc::ServerContext* context, const SayHelloStreamReplyRequest* request, grpc::ServerWriter<SayHelloStreamReplyResponse>* writer) override {
        return grpc::Status::OK;
    }

    grpc::Status SayHelloBidiStream(grpc::ServerContext* context, grpc::ServerReaderWriter<SayHelloBidiStreamResponse, SayHelloBidiStreamRequest>* stream) override {
        return grpc::Status::OK;
    }
};
