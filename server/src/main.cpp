#include <print>
#include <Sane.hpp>
extern "C" {
#include <sane/sane.h>
}

int main(int argc, char* argv[]) {
    sane::CSane sane{};
    if (sane.is_ok()) {
        sane.get_devices();
    }

    std::println("Hello world server!");

    return 0;
}
