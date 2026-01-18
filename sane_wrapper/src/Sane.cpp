#include "../include/Sane.hpp"
#include "../include/SaneDevice.hpp"

#include <algorithm>
#include <cstdint>
#include <expected>
#include <memory>
#include <vector>

extern "C" {
#include <sane/sane.h>
}

sane::CSane::CSane(SANE_Int* version_code, SANE_Auth_Callback auth_callback) : m_version_code(version_code), m_auth_callback(auth_callback) {};
sane::CSane::~CSane() {
    sane_exit();
}
sane::CSane::CSane(const sane::CSane&& other) :
    m_version_code(std::move(other.m_version_code)), m_auth_callback(std::move(other.m_auth_callback)), m_devices(std::move(other.m_devices)),
    m_devices_weak(std::move(other.m_devices_weak)) {}

std::expected<sane::CSane, SANE_Status> sane::CSane::create_instance(SANE_Int* version_code, SANE_Auth_Callback auth_callback) {
    sane::CSane sane_instance(version_code, auth_callback);

    auto        status = sane_instance.init();

    if (status != SANE_STATUS_GOOD) {
        return std::unexpected(status);
    }

    return sane_instance;
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
