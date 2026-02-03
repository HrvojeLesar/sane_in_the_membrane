#include "../include/Sane.hpp"
#include "../include/SaneDevice.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

extern "C" {
#include <sane/sane.h>
}

using namespace sane_in_the_membrane;

sane::CSane::CSane(SANE_Auth_Callback auth_callback) : m_auth_callback(auth_callback) {
    init();
};

sane::CSane::~CSane() {
    sane_exit();
}

sane::CSaneStatus sane::CSane::init() {
    m_initialization_status = sane_init(&m_version_code, m_auth_callback);

    return m_initialization_status.value();
}

// TODO: Add handling for multiple threads, invalidate m_devices before sane_get_devices is called
std::vector<std::weak_ptr<sane::CSaneDevice>> sane::CSane::get_devices(SANE_Bool local_only) {
    for (const auto& device : m_devices) {
        device->clear_raw_device();
    }

    m_devices.clear();

    const SANE_Device** device_list;
    auto                status = sane_get_devices(&device_list, local_only);

    if (status == SANE_STATUS_GOOD) {
        for (uint32_t i = 0; device_list[i] != nullptr; i++) {
            auto device = device_list[i];

            auto device_p = std::make_unique<CSaneDevice>(device);
            device_p->print_info();

            m_devices.emplace_back(std::move(device_p));
        }
    }

    m_devices_weak.clear();
    m_devices_weak.reserve(m_devices.size());

    for (const auto& device_p : m_devices) {
        m_devices_weak.emplace_back(device_p);
    }

    return m_devices_weak;
}

const std::vector<std::weak_ptr<sane::CSaneDevice>> sane::CSane::peek_devices(SANE_Bool local_only) {
    return m_devices_weak;
}

std::optional<sane::CSaneStatus> sane::CSane::get_status() const {
    return m_initialization_status;
}

bool sane::CSane::is_ok() const {
    return m_initialization_status == SANE_STATUS_GOOD;
}
