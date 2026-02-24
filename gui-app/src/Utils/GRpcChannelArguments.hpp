#ifndef UTILS_GRPC_CHANNEL_ARGUMENTS
#define UTILS_GRPC_CHANNEL_ARGUMENTS

#include <grpc/compression.h>
#include <grpcpp/support/channel_arguments.h>
namespace sane_in_the_membrane::utils {
    grpc::ChannelArguments default_channel_args();
}

#endif // !UTILS_GRPC_CHANNEL_ARGUMENTS
