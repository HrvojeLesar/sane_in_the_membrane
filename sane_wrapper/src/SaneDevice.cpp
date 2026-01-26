#include "../include/SaneDevice.hpp"
#include <iostream>
#include <print>

sane::CSaneDevice::CSaneDevice(const SANE_Device* device) : m_raw_device(device), m_device_name(device->name) {}
sane::CSaneDevice::CSaneDevice(const char* device_name) : m_raw_device(nullptr), m_device_name(device_name) {}
sane::CSaneDevice::~CSaneDevice() {}

SANE_Status sane::CSaneDevice::open() {
    auto status = sane_open(m_device_name.c_str(), &m_handle);

    if (status == SANE_STATUS_GOOD) {
        m_current_state = EState::OPENED;
    }

    return status;
}

void sane::CSaneDevice::close() {
    m_option_descriptors.clear();
    if (m_handle == nullptr) {
        return;
    }

    // Driver should handle canceling current action for the active scan
    sane_close(m_handle);

    m_handle = nullptr;

    m_current_state = EState::CLOSED;
}

const SANE_Option_Descriptor* sane::CSaneDevice::get_option_descriptor(SANE_Int option_number) {
    if (m_handle == nullptr || m_current_state != EState::OPENED) {
        return nullptr;
    }

    auto descriptor = sane_get_option_descriptor(m_handle, option_number);

    if (descriptor != nullptr) {
        m_option_descriptors.emplace_back(option_number, descriptor);
    }

    if (option_number == 0) {
        m_max_options_n = descriptor->size;
    }

    return descriptor;
}

SANE_Status sane::CSaneDevice::control_option(SANE_Int option_number, SANE_Action action, void* data, SANE_Int* info) {
    if (m_handle == nullptr || m_current_state != EState::OPENED) {
        return SANE_STATUS_INVAL;
    }

    return sane_control_option(m_handle, option_number, action, data, info);
}

SANE_Status sane::CSaneDevice::get_parameters() {
    if (m_handle == nullptr) {
        return SANE_STATUS_INVAL;
    }

    return sane_get_parameters(m_handle, &m_parameters);
}

SANE_Status sane::CSaneDevice::start() {
    if (m_handle == nullptr || m_current_state != EState::OPENED) {
        return SANE_STATUS_INVAL;
    }

    auto status = sane_start(m_handle);

    if (status == SANE_STATUS_GOOD) {
        m_current_state = EState::STARTED;
    }

    return status;
}

SANE_Status sane::CSaneDevice::read() {
    return read(m_buffer);
}

template <std::size_t N>
SANE_Status sane::CSaneDevice::read(sane::CSaneDeviceBuffer<N>& buffer) {
    if (m_handle == nullptr || m_current_state != EState::STARTED) {
        return SANE_STATUS_INVAL;
    }

    auto status = sane_read(m_handle, buffer.m_data.data(), buffer.m_data.max_size(), &buffer.read_len);

    return status;
}

void sane::CSaneDevice::cancel() {
    auto old_state  = m_current_state;
    m_current_state = EState::CANCELLING;

    sane_cancel(m_handle);

    m_current_state = old_state;
}

SANE_Status sane::CSaneDevice::set_io_mode(SANE_Bool is_none_blocking) {
    return sane_set_io_mode(m_handle, is_none_blocking);
}

SANE_String_Const sane::CSaneDevice::str_status(SANE_Status status) {
    return sane_strstatus(status);
}

void sane::CSaneDevice::print_info() {
    if (m_raw_device == nullptr) {
        return;
    }
    std::println("Device: {}, {}, {}, {}", m_raw_device->model, m_raw_device->name, m_raw_device->type, m_raw_device->vendor);
}

const std::string& sane::CSaneDevice::get_name() const {
    return m_device_name;
}

void sane::CSaneDevice::clear_raw_device() {
    m_raw_device = nullptr;
}

const SANE_Device* sane::CSaneDevice::get_raw_device() const {
    return m_raw_device;
}
