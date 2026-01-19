#pragma once
#ifndef SANE_IN_THE_MEMBRANE_DEVICE_BUFFER
#define SANE_IN_THE_MEMBRANE_DEVICE_BUFFER

extern "C" {
#include <sane/sane.h>
}
#include <cstddef>
#include <vector>

namespace sane {
    class CSaneDeviceBuffer {
      public:
        CSaneDeviceBuffer();
        CSaneDeviceBuffer(std::size_t reserve);
        ~CSaneDeviceBuffer();

        std::vector<SANE_Byte> m_data{};

        SANE_Int               max_len() const;
        SANE_Int               read_len = 0;
        void                   resize();
        void                   resize(std::size_t size);
    };

}

#endif // !SANE_IN_THE_MEMBRANE_DEVICE_BUFFER
