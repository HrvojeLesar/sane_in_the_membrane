#include "SaneMock.hpp"

#include <iostream>
#include <sane/sane.h>
#include <algorithm>
#include <cstring>
#include <format>

#include <Assert.hpp>

using namespace sane_in_the_membrane::mock;

SMockDevice::SMockDevice() {}

SMockDevice::SMockDevice(size_t number) :
    name(std::format("MOCK_NAME_{}", number)), vendor(std::format("MOCK_VENDOR_{}", number)), model(std::format("MOCK_MODEL_{}", number)),
    type(std::format("MOCK_SCANNER_TYPE_{}", number)) {
    device.name   = name.c_str();
    device.vendor = vendor.c_str();
    device.model  = model.c_str();
    device.type   = type.c_str();
}

CMockHandle::CMockHandle(SMockDevice* device) : m_device(device) {}

CMockHandle::CMockHandle(const CMockHandle& handle, const SMockDevice* device) : m_option_descriptor(handle.m_option_descriptor), m_open(handle.m_open), m_device(device) {}

void CMockHandle::set_max_read_bytes(size_t max_length) {
    m_bytes_read      = 0;
    m_max_read_length = max_length;
}

size_t CMockHandle::get_max_read_bytes() {
    return m_max_read_length;
}

size_t CMockHandle::get_bytes_read() {
    return m_bytes_read;
}

size_t CMockHandle::get_read_bytes_capacity() {
    if (m_bytes_read > m_max_read_length)
        return 0;

    return m_max_read_length - m_bytes_read;
}

void CMockHandle::set_bytes_read(size_t bytes) {
    m_bytes_read = bytes;
}

CSaneState::CSaneState() {}

CSaneState::~CSaneState() {
    cleanup_device_list();
}

CSaneState& CSaneState::instance() {
    static CSaneState instance{};

    return instance;
}

void CSaneState::add_device(const SMockDevice& device) {
    m_devices.emplace_back(std::make_unique<SMockDevice>(device));
}

void CSaneState::add_handle(const CMockHandle& mock_handle) {
    m_devices.emplace_back(std::make_unique<SMockDevice>());

    auto mock_device    = m_devices.back().get();
    mock_device->device = mock_handle.m_device->device;
    mock_device->name   = mock_handle.m_device->name;
    mock_device->vendor = mock_handle.m_device->vendor;
    mock_device->model  = mock_handle.m_device->model;
    mock_device->type   = mock_handle.m_device->type;

    m_handles.emplace_back(std::make_unique<CMockHandle>(mock_handle, mock_device));
}

void CSaneState::generate_mock_device() {
    m_devices.emplace_back(std::make_unique<SMockDevice>(m_devices.size()));
}

CMockHandle* CSaneState::get_handle(const std::string& device_name) const {
    auto handle_iter =
        std::find_if(m_handles.begin(), m_handles.end(), [&device_name](const std::unique_ptr<CMockHandle>& handle) { return handle->m_device->name == device_name; });
    if (handle_iter != m_handles.end())
        return (*handle_iter).get();

    return nullptr;
}

CMockHandle* CSaneState::get_handle(const SANE_Handle handle) const {
    auto handle_iter = std::find_if(m_handles.begin(), m_handles.end(), [&handle](const std::unique_ptr<CMockHandle>& mock_handle) { return mock_handle.get() == handle; });
    if (handle_iter != m_handles.end())
        return (*handle_iter).get();

    return nullptr;
}

CMockHandle* CSaneState::get_or_create_handle(const std::string& device_name) {
    auto handle = get_handle(device_name);
    if (handle)
        return handle;

    auto device_iter = std::find_if(m_devices.begin(), m_devices.end(), [&device_name](const std::unique_ptr<SMockDevice>& device) { return device->name == device_name; });
    if (device_iter == m_devices.end())
        return nullptr;

    auto device = (*device_iter).get();

    m_handles.emplace_back(std::make_unique<CMockHandle>(device));

    return m_handles.back().get();
}

const SANE_Device** CSaneState::sane_comptabile_device_list() {
    cleanup_device_list();

    auto device_count             = m_devices.size();
    m_sane_compatible_device_list = new const SANE_Device* [device_count + 1] {};

    for (size_t index = 0; index < device_count; ++index)
        m_sane_compatible_device_list[index] = &m_devices[index].get()->device;

    m_sane_compatible_device_list[device_count] = nullptr;

    return m_sane_compatible_device_list;
}

void CSaneState::cleanup_device_list() {
    if (m_sane_compatible_device_list)
        delete m_sane_compatible_device_list;

    m_sane_compatible_device_list = nullptr;
}

extern "C" {
SANE_Status sane_init(SANE_Int* version_code, SANE_Auth_Callback authorize) {
    CSaneState::instance().m_initialized = true;

    return SANE_STATUS_GOOD;
}

void sane_exit(void) {
    CSaneState::instance().m_initialized = false;
    CSaneState::instance().cleanup_device_list();
}

SANE_Status sane_get_devices(const SANE_Device*** device_list, SANE_Bool local_only) {
    SITM_ASSERT(CSaneState::instance().m_initialized, "Sane is not initialized");

    *device_list = CSaneState::instance().sane_comptabile_device_list();

    return SANE_STATUS_GOOD;
}

SANE_Status sane_open(SANE_String_Const devicename, SANE_Handle* handle) {

    // As a special case, specifying a zero-length string as the device requests
    // opening the first available device (if there is such a device).
    if (std::char_traits<char>::length(devicename) == 0)
        devicename = CSaneState::instance().m_devices.front().get()->name.c_str();

    auto mock_handle = CSaneState::instance().get_or_create_handle(devicename);

    *handle = mock_handle;

    if (!handle)
        return SANE_STATUS_INVAL;

    mock_handle->m_open = true;
    return SANE_STATUS_GOOD;
}

void sane_close(SANE_Handle handle) {
    auto mock_handle = CSaneState::instance().get_handle(handle);
    if (mock_handle) {
        SITM_ASSERT(mock_handle->m_cancelled, "Device action was not cancelled before closing.");
        mock_handle->m_open      = false;
        mock_handle->m_cancelled = false;
    }
}

const SANE_Option_Descriptor* sane_get_option_descriptor(SANE_Handle handle, SANE_Int option) {
    auto mock_handle = CSaneState::instance().get_handle(handle);
    if (!mock_handle)
        return nullptr;

    return &mock_handle->m_option_descriptor;
}

SANE_Status sane_control_option(SANE_Handle handle, SANE_Int option, SANE_Action action, void* value, SANE_Int* info) {
    return SANE_STATUS_UNSUPPORTED;
}

SANE_Status sane_get_parameters(SANE_Handle handle, SANE_Parameters* params) {
    auto mock_handle = CSaneState::instance().get_handle(handle);
    if (!mock_handle)
        return SANE_STATUS_INVAL;

    *params = mock_handle->m_parameters;

    return SANE_STATUS_GOOD;
}

SANE_Status sane_start(SANE_Handle handle) {
    auto mock_handle = CSaneState::instance().get_handle(handle);
    if (!mock_handle)
        return SANE_STATUS_INVAL;

    mock_handle->m_started   = true;
    mock_handle->m_cancelled = false;

    return SANE_STATUS_GOOD;
}

SANE_Status sane_read(SANE_Handle handle, SANE_Byte* data, SANE_Int max_length, SANE_Int* length) {
    auto mock_handle = CSaneState::instance().get_handle(handle);
    if (!mock_handle)
        return SANE_STATUS_INVAL;

    SITM_ASSERT(mock_handle->m_started, "Invalid read. Reading from a device that has not started.");

    if (mock_handle->get_read_bytes_capacity() == 0)
        return SANE_STATUS_EOF;

    size_t write_bytes = max_length;
    size_t capacity    = mock_handle->get_read_bytes_capacity();
    if (write_bytes > capacity)
        write_bytes = capacity;

    mock_handle->set_bytes_read(mock_handle->get_bytes_read() + write_bytes);

    std::memset(data, 1, sizeof(SANE_Byte));

    if (length)
        *length = write_bytes;

    return SANE_STATUS_GOOD;
}

void sane_cancel(SANE_Handle handle) {
    auto mock_handle = CSaneState::instance().get_handle(handle);
    if (mock_handle) {
        mock_handle->m_started   = false;
        mock_handle->m_cancelled = true;
    }
}

SANE_Status sane_set_io_mode(SANE_Handle handle, SANE_Bool non_blocking) {
    return SANE_STATUS_UNSUPPORTED;
}

SANE_Status sane_get_select_fd(SANE_Handle handle, SANE_Int* fd) {
    return SANE_STATUS_UNSUPPORTED;
}

SANE_String_Const sane_strstatus(SANE_Status status) {
    switch (status) {
        case SANE_STATUS_GOOD: return "GOOD";
        case SANE_STATUS_UNSUPPORTED: return "UNSUPPORTED";
        case SANE_STATUS_CANCELLED: return "CANCELLED";
        case SANE_STATUS_DEVICE_BUSY: return "DEVICE_BUSY";
        case SANE_STATUS_INVAL: return "INVAL";
        case SANE_STATUS_EOF: return "EOF";
        case SANE_STATUS_JAMMED: return "JAMMED";
        case SANE_STATUS_NO_DOCS: return "NO_DOCS";
        case SANE_STATUS_COVER_OPEN: return "COVER_OPEN";
        case SANE_STATUS_IO_ERROR: return "IO_ERROR";
        case SANE_STATUS_NO_MEM: return "NO_MEM";
        case SANE_STATUS_ACCESS_DENIED: return "ACCESS_DENIED";
        default: return "UNKNOWN";
    }
}
}
