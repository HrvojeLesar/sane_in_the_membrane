#pragma once
#ifndef SANE_IN_THE_MEMBRANE_DEVICE
#define SANE_IN_THE_MEMBRANE_DEVICE

extern "C" {
#include <sane/sane.h>
}
#include <vector>
#include "SaneDeviceBuffer.hpp"

namespace sane {
    class CSaneDevice {

        enum class EState {
            OPENED,
            CLOSED,
            STARTED,
            CANCELLING,
        };

      public:
        CSaneDevice(const SANE_Device* device);
        ~CSaneDevice();

        SANE_Status                   open();
        void                          close();
        const SANE_Option_Descriptor* get_option_descriptor(SANE_Int option_number);
        SANE_Status                   control_option(SANE_Int option_number, SANE_Action action, void* data, SANE_Int* info);
        SANE_Status                   get_parameters();
        SANE_Status                   start();
        SANE_Status                   read();
        SANE_Status                   read(sane::CSaneDeviceBuffer& buffer);
        void                          cancel();
        SANE_Status                   set_io_mode(SANE_Bool is_none_blocking);
        SANE_String_Const             str_status(SANE_Status status);
        void                          print_info();

        const SANE_Device*            m_raw_device{};

      private:
        SANE_Handle                                                     m_handle{};
        SANE_Int                                                        m_max_options_n{};
        std::vector<std::pair<SANE_Int, const SANE_Option_Descriptor*>> m_option_descriptors{};
        SANE_Parameters                                                 m_parameters{};
        EState                                                          m_current_state{EState::CLOSED};
        sane::CSaneDeviceBuffer                                         m_buffer{2048};
    };
}

#endif // !SANE_IN_THE_MEMBRANE_DEVICE
