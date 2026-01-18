#include "../include/Sane.hpp"
#include "../include/SaneDevice.hpp"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

extern "C" {
#include <sane/sane.h>
}

sane::CSane::CSane(SANE_Int* version_code, SANE_Auth_Callback auth_callback) : m_version_code(version_code), m_auth_callback(auth_callback) {};
sane::CSane::~CSane() {
    sane_exit();
}

std::optional<sane::CSane> sane::CSane::create_instance(SANE_Int* version_code, SANE_Auth_Callback auth_callback) {
    CSane saneInstance(version_code, auth_callback);

    if (!saneInstance.init()) {
        saneInstance;
    }

    return std::nullopt;
}

SANE_Status sane::CSane::init() {
    return sane_init(m_version_code, m_auth_callback);
}

// TODO: Add handling for multiple threads, invalidate m_devices before sane_get_devices is called
std::vector<std::weak_ptr<sane::CSaneDevice>> sane::CSane::get_devices(SANE_Bool local_only) {
    const SANE_Device** device_list;

    // WARN: sane_get_devices and sane_exit invalidate all existing m_devices
    // Queue or lock any sane_get_devices and sane_exit calls
    // Before calling sane_exit all
    auto status = sane_get_devices(&device_list, local_only);

    if (status == SANE_STATUS_GOOD) {
        m_devices.clear();
        for (uint32_t i = 0; device_list[i] != nullptr; i++) {
            auto device = device_list[i];

            m_devices.emplace_back(std::make_shared<CSaneDevice>(CSaneDevice{device}));
        }
    }

    m_devices_weak.clear();
    m_devices_weak.reserve(m_devices.size());

    std::ranges::transform(m_devices.begin(), m_devices.end(), m_devices_weak.begin(),
                           [](std::shared_ptr<sane::CSaneDevice> device) { return std::weak_ptr<sane::CSaneDevice>(device); });

    return m_devices_weak;
}

const std::vector<std::weak_ptr<sane::CSaneDevice>> sane::CSane::peek_devices(SANE_Bool local_only) {
    return m_devices_weak;
}

void t() {
    int  test = 123;
    auto sane = sane::CSane::create_instance(&test);

    sane->get_devices();
}
