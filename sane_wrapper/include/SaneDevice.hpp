#pragma once
#ifndef SANE_IN_THE_MEMBRANE_DEVICE
#define SANE_IN_THE_MEMBRANE_DEVICE

extern "C" {
#include <sane/sane.h>
}
#include <vector>
#include <string>
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
        CSaneDevice(const char* device_name);
        ~CSaneDevice();

        SANE_Status                   open();
        void                          close();
        const SANE_Option_Descriptor* get_option_descriptor(SANE_Int option_number);
        SANE_Status                   control_option(SANE_Int option_number, SANE_Action action, void* data, SANE_Int* info);
        SANE_Status                   get_parameters();
        SANE_Status                   start();
        SANE_Status                   read();

        template <std::size_t N>
        SANE_Status        read(sane::CSaneDeviceBuffer<N>& buffer);
        void               cancel();
        SANE_Status        set_io_mode(SANE_Bool is_none_blocking);
        SANE_String_Const  str_status(SANE_Status status);
        void               print_info();

        const std::string& get_name() const;
        const SANE_Device* get_raw_device() const;
        void               clear_raw_device();

      public:
        const SANE_Parameters& parameters() const {
            return m_parameters;
        }

      private:
        const SANE_Device*                                              m_raw_device{};
        SANE_Handle                                                     m_handle{};
        SANE_Int                                                        m_max_options_n{};
        std::vector<std::pair<SANE_Int, const SANE_Option_Descriptor*>> m_option_descriptors{};
        SANE_Parameters                                                 m_parameters{};
        EState                                                          m_current_state{EState::CLOSED};
        sane::CSaneDeviceBuffer<2048>                                   m_buffer{};
        std::string                                                     m_device_name{};
    };
}

#endif // !SANE_IN_THE_MEMBRANE_DEVICE
