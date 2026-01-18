#pragma once
#ifndef SANE_IN_THE_MEMBRANE_DEVICE
#define SANE_IN_THE_MEMBRANE_DEVICE

extern "C" {
#include <sane/sane.h>
}

namespace sane {
    class CSaneDevice {
      public:
        CSaneDevice(const SANE_Device* device);

      private:
        const SANE_Device* m_raw_device;
    };
}

#endif // !SANE_IN_THE_MEMBRANE_DEVICE
