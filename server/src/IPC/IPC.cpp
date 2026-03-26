#include "IPC.hpp"
#include <cerrno>
#include <cstring>
#include <format>
#include <sys/poll.h>
#include <sys/socket.h>
#include <Assert.hpp>
#include <GLogger.hpp>
#include <sys/un.h>
#include <netinet/in.h>
#include <pwd.h>
#include <unistd.h>

using namespace sane_in_the_membrane;
using namespace sane_in_the_membrane::ipc;

void CIPC::start_blocking() {
    const auto socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        log::warn("Failed to start IPC socket. IPC will not work. Error: {}", strerror(errno));
        return;
    }

    const auto socket_path = ipc_path();

    unlink(socket_path.c_str());

    sockaddr_un socket_address = {.sun_family = AF_UNIX};
    strcpy(socket_address.sun_path, socket_path.c_str());

    if (bind(socket_fd, (sockaddr*)&socket_address, SUN_LEN(&socket_address)) < 0) {
        log::warn("Failed to bind IPC socket. IPC will not work. Error: {}", strerror(errno));
        return;
    }

    if (listen(socket_fd, LISTEN_QUEUE_NUM) < 0) {
        log::warn("Failed to listen on IPC socket. IPC will not work. Error: {}", strerror(errno));
        return;
    }

    sockaddr_in client_address                = {};
    socklen_t   client_socket_len             = sizeof(client_address);
    char        read_buffer[READ_BUFFER_SIZE] = {0};

    log::debug("IPC socket started at {} (fd: {})", socket_path, socket_fd);
    while (!m_shutdown) {
        pollfd pfd{
            .fd     = socket_fd,
            .events = POLLIN,
        };

        const auto poll_result = poll(&pfd, 1, 500);
        if (poll_result <= 0) {
            continue;
        }

        if (pfd.events & POLLIN) {
            const auto client_connection_fd = accept(socket_fd, (sockaddr*)&client_address, &client_socket_len);
            if (client_connection_fd < 0) {
                log::warn("Failed to accept on IPC socket. Error: {}", strerror(errno));
                continue;
            }

            log::debug("Accepted incoming connection on socket, client fd: {}", client_connection_fd);
            while (true) {
                auto message_size = read(client_connection_fd, read_buffer, READ_BUFFER_SIZE);
                if (message_size < 0) {
                    log::warn("Failed to read from IPC socket. Error: {}", strerror(errno));
                    break;
                }

                read_buffer[message_size == READ_BUFFER_SIZE ? (READ_BUFFER_SIZE - 1) : message_size] = '\0';
                if (message_size == 0)
                    break;

                std::string request{read_buffer};
                const auto  response = process_request(request);

                write(client_connection_fd, response.c_str(), response.length());
            }

            log::debug("Closing Accepted Connection");
            close(client_connection_fd);
        }
    }

    close(socket_fd);
}

void CIPC::shutdown() {
    m_shutdown = true;
}

const std::string CIPC::ipc_path() const {
    const auto userid = std::to_string(getpwuid(getuid())->pw_uid);

    const auto base_dir = "/run/user/" + userid + "/sane_in_the_membrane";

    if (!std::filesystem::exists(base_dir))
        std::filesystem::create_directories(base_dir);

    return base_dir + "/sitm.sock";
}

const std::string CIPC::process_request(std::string& request) const {
    log::debug("User requested: {}", request);

    if (request.find("port") == 0) {
        return std::format("Current port is: `{}`", *m_server_port.shared_access());
    }

    return "Unknown request";
}

void CIPC::set_port(int port) {
    auto port_access = m_server_port.access();
    *port_access     = port;
}
