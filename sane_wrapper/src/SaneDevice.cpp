#include "../include/SaneDevice.hpp"
extern "C" {
#include <sane/sane.h>
}

sane::CSaneDevice::CSaneDevice(const SANE_Device* device) : m_raw_device(device) {}
