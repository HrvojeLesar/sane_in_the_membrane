#include "ServerSocket.hpp"

#include <cerrno>
#include <cstdint>
#include <netdb.h>
#include <print>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>
#include <netinet/tcp.h>
#include <vector>

bool bind_socket(int socket_fd, addrinfo& ai) {
    return bind(socket_fd, ai.ai_addr, ai.ai_addrlen) != 0 ? true : false;
}

bool listen_on_socket(int socket_fd, addrinfo& ai) {
    return listen(socket_fd, 2) != 0 ? true : false;
}

void poll_my_pole(int connection_fd) {
    while (true) {
        pollfd pfd;
        pfd.fd     = connection_fd;
        pfd.events = POLLIN;

        std::println("Polling");
        auto result = poll(&pfd, 1, -1);
        std::println("Polled");
        if (result == -1) {
            std::println("Failed polling");
            break;
        }

        std::println("Reading");
        std::vector<char> read_buffer(1024 * 4, 0);
        auto              byte_count = recv(connection_fd, read_buffer.data(), 1024 * 4, 0);

        for (auto i = 0; i < byte_count; ++i) {
            std::print("{}", read_buffer.at(i));
        }
        std::println("finished reading");
    }
}

server::Connection::ServerSocket::ServerSocket() {}
server::Connection::ServerSocket::~ServerSocket() {}

void server::Connection::ServerSocket::listen_for_connection() {
    const std::string localhost{"localhost"};
    const std::string port{"6969"};
    struct addrinfo   hints{
          .ai_flags    = 0,
          .ai_family   = AF_UNSPEC,
          .ai_socktype = SOCK_STREAM,
          .ai_protocol = IPPROTO_IP,
    };
    struct addrinfo* result{};

    if (auto error_val = getaddrinfo(localhost.c_str(), port.c_str(), &hints, &result)) {
        std::println("getaddrinfo failed with error: {}", gai_strerror(error_val));
        return;
    }

    int socket_fd = -1;
    for (auto ai = result; ai; ai = ai->ai_next) {
        socket_fd = socket(ai->ai_family, ai->ai_socktype | SOCK_CLOEXEC, ai->ai_protocol);

        if (socket_fd == -1) {
            continue;
        }

        int32_t opt = 1;
        setsockopt(socket_fd, IPPROTO_IP, TCP_NODELAY, &opt, sizeof(opt));

        if (socket_fd == -1) {
            std::println("Failed to create socket fd");
            return;
        }

        if (bind_socket(socket_fd, *ai)) {
            close(socket_fd);
        }

        if (listen_on_socket(socket_fd, *ai)) {
            close(socket_fd);
        }

        break;
    }

    freeaddrinfo(result);

    if (socket_fd == -1) {
        std::println("Socket has not been constructed");
        return;
    }

    std::println("Socket je gucci");

    std::vector<std::jthread> threads{};

    while (true) {
        auto incoming_connection_fd = accept4(socket_fd, nullptr, nullptr, SOCK_CLOEXEC);

        std::println("errno: {}", errno);

        std::println("Got a connection on socket: {}", incoming_connection_fd);

        if (incoming_connection_fd == -1) {
            break;
        }

        threads.emplace_back(poll_my_pole, incoming_connection_fd);
    }

    close(socket_fd);
}
