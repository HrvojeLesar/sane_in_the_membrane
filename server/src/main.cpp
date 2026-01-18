#include <expected>
#include <print>
#include <Sane.hpp>
extern "C" {
#include <sane/sane.h>
}

void callback(SANE_String_Const resource, SANE_Char username[SANE_MAX_USERNAME_LEN], SANE_Char password[SANE_MAX_PASSWORD_LEN]) {
    std::println("callback");
}

int main(int argc, char* argv[]) {
    SANE_Int version_code;

    auto     sane = sane::CSane::create_instance(&version_code, callback);

    if (!sane.has_value()) {
        SANE_Status err = sane.error();
        std::println("Failed to create sane instance: {}", (int)err);
    } else {
        std::println("WAS");
        auto devices = sane->get_devices();
        std::println("WAS {}", devices.size());
    }

    std::println("Hello world server!");

    return 0;
}
