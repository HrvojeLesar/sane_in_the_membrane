#pragma once
#include <memory>
#ifndef SANE_IN_THE_MEMBRANE
#define SANE_IN_THE_MEMBRANE

extern "C" {
#include <sane/sane.h>
}

#include <optional>
#include <vector>
#include "SaneDevice.hpp"

namespace sane {
    class CSane {
      private:
        CSane(SANE_Int* version_code, SANE_Auth_Callback auth_callback = nullptr);

      public:
        static std::optional<CSane> create_instance(SANE_Int* version_code, SANE_Auth_Callback auth_callback = nullptr);
        ~CSane();

        CSane(const CSane&) noexcept  = delete;
        CSane(const CSane&&) noexcept = delete;

        std::vector<std::weak_ptr<CSaneDevice>>       get_devices(SANE_Bool local_only = false);
        const std::vector<std::weak_ptr<CSaneDevice>> peek_devices(SANE_Bool local_only = false);

      private:
        SANE_Int*                                 m_version_code  = nullptr;
        SANE_Auth_Callback                        m_auth_callback = nullptr;
        std::vector<std::shared_ptr<CSaneDevice>> m_devices{};
        std::vector<std::weak_ptr<CSaneDevice>>   m_devices_weak{};
        SANE_Status                               init();
    };

}

#endif // !SANE_IN_THE_MEMBRANE
