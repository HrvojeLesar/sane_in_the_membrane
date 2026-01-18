#pragma once
#ifndef SANE_IN_THE_MEMBRANE
#define SANE_IN_THE_MEMBRANE

extern "C" {
#include <sane/sane.h>
}

#include "SaneDevice.hpp"
#include <vector>
#include <memory>

namespace sane {
    class CSane {
      public:
        CSane(SANE_Auth_Callback auth_callback = nullptr);
        ~CSane();

        CSane(const CSane&) noexcept  = delete;
        CSane(const CSane&&) noexcept = delete;

        std::vector<std::weak_ptr<CSaneDevice>>       get_devices(SANE_Bool local_only = false);
        const std::vector<std::weak_ptr<CSaneDevice>> peek_devices(SANE_Bool local_only = false);
        SANE_Status                                   get_status() const;

      private:
        SANE_Int                                  m_version_code{};
        SANE_Auth_Callback                        m_auth_callback = nullptr;
        std::vector<std::shared_ptr<CSaneDevice>> m_devices{};
        std::vector<std::weak_ptr<CSaneDevice>>   m_devices_weak{};
        SANE_Status                               m_initialization_status{};
    };

}

#endif // !SANE_IN_THE_MEMBRANE
