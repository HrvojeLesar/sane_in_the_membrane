#ifndef SANE_IN_THE_MEMBRANE_DEVICE_BUFFER
#define SANE_IN_THE_MEMBRANE_DEVICE_BUFFER

#include <array>
extern "C" {
#include <sane/sane.h>
}
#include <cstddef>

namespace sane {
    template <std::size_t N = 2048>
    class CSaneDeviceBuffer {
      public:
        CSaneDeviceBuffer() {}
        CSaneDeviceBuffer(const CSaneDeviceBuffer& other) : m_data{other.m_data} {}
        CSaneDeviceBuffer(const CSaneDeviceBuffer&& other) : m_data{std::move(other.m_data)} {}

        CSaneDeviceBuffer& operator=(const CSaneDeviceBuffer& other) {
            this->m_data = other.m_data;

            return *this;
        }
        CSaneDeviceBuffer& operator=(const CSaneDeviceBuffer&& other) {
            this->m_data = std::move(other.m_data);

            return *this;
        }

        ~CSaneDeviceBuffer() {}

        std::array<SANE_Byte, N> m_data{};
        SANE_Int                 read_len = 0;
    };

}

#endif // !SANE_IN_THE_MEMBRANE_DEVICE_BUFFER
