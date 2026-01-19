#include "../include/SaneDeviceBuffer.hpp"

sane::CSaneDeviceBuffer::CSaneDeviceBuffer() {}

sane::CSaneDeviceBuffer::CSaneDeviceBuffer(std::size_t reserve) {
    m_data.reserve(reserve);
}

sane::CSaneDeviceBuffer::~CSaneDeviceBuffer() {}

SANE_Int sane::CSaneDeviceBuffer::max_len() const {
    return m_data.capacity() - m_data.size();
}

void sane::CSaneDeviceBuffer::resize() {
    m_data.reserve(m_data.capacity() * 2);
}

void sane::CSaneDeviceBuffer::resize(std::size_t size) {
    m_data.reserve(m_data.size() + size);
}
