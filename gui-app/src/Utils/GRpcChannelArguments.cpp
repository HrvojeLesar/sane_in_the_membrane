#include "GRpcChannelArguments.hpp"

grpc::ChannelArguments sane_in_the_membrane::utils::default_channel_args() {
    grpc::ChannelArguments args{};
    args.SetMaxReceiveMessageSize(50 * 1024 * 1024);
    args.SetCompressionAlgorithm(GRPC_COMPRESS_GZIP);

    return args;
}
