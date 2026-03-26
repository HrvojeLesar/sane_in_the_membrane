#include <cstring>
#include <print>
#include <string_view>
#include <sys/socket.h>
#include <sys/un.h>
#include <GLogger.hpp>
#include <pwd.h>
#include <unistd.h>

#define READ_BUFFER_SIZE 1024

using namespace sane_in_the_membrane;

const std::string ipc_path() {
    const auto userid = std::to_string(getpwuid(getuid())->pw_uid);

    const auto base_dir = "/run/user/" + userid + "/sane_in_the_membrane";

    if (!std::filesystem::exists(base_dir))
        std::filesystem::create_directories(base_dir);

    return base_dir + "/sitm.sock";
}

void send_message(std::string& message) {
    struct sockaddr_un server_addr{.sun_family = AF_UNIX};

    const auto         server_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket_fd == -1) {
        log::error("Failed to open unix socket");
        exit(EXIT_FAILURE);
    }

    const auto socket_path = ipc_path();

    strcpy(server_addr.sun_path, socket_path.c_str());

    if (connect(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_un)) < 0) {
        log::error("Failed to connect to server socket. Is socket available ?");
        exit(EXIT_FAILURE);
    }

    auto bytes_sent = send(server_socket_fd, message.c_str(), message.length(), 0);
    if (bytes_sent < 0) {
        log::error("Failed to send message");
        close(server_socket_fd);
        exit(EXIT_FAILURE);
    }

    char read_buffer[READ_BUFFER_SIZE] = {0};
    auto bytes_received                = recv(server_socket_fd, &read_buffer, sizeof(read_buffer), 0);
    if (bytes_received < 0) {
        log::error("Failed to read response");
        close(server_socket_fd);
        exit(EXIT_FAILURE);
    }

    std::string received_message(read_buffer, bytes_received);

    std::println("{}", received_message);

    close(server_socket_fd);
}

int main(int argc, char* argv[]) {
    std::string message{"port"};

    send_message(message);
    return 0;
}
