#include <cstddef>
#include <print>
#include <Sane.hpp>
extern "C" {
#include <sane/sane.h>
}

#include "Connection/ServerSocket.hpp"

int main(int argc, char* argv[]) {
    std::size_t thread_count{5};
    while (thread_count > 0) {
        std::println("Count {}", thread_count);
        --thread_count;
    }

    server::Connection::ServerSocket s{};
    s.listen_for_connection();

    return 0;
}
