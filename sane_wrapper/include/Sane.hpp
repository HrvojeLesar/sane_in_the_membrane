#pragma once
#ifndef SANE_IN_THE_MEMBRANE
#define SANE_IN_THE_MEMBRANE

extern "C" {
#include <sane/sane.h>
}

#include "SaneDevice.hpp"
#include "SaneStatus.hpp"
#include <vector>
#include <memory>
#include <optional>

namespace sane {
    class CSane {
      public:
        CSane(SANE_Auth_Callback auth_callback = nullptr);
        ~CSane();

        CSane(const CSane&) noexcept  = delete;
        CSane(const CSane&&) noexcept = delete;

        std::vector<std::weak_ptr<CSaneDevice>>       get_devices(SANE_Bool local_only = false);
        const std::vector<std::weak_ptr<CSaneDevice>> peek_devices(SANE_Bool local_only = false);
        std::optional<CSaneStatus>                    get_status() const;
        bool                                          is_ok() const;
        CSaneStatus                                   init();

      private:
        SANE_Int                                  m_version_code{};
        SANE_Auth_Callback                        m_auth_callback = nullptr;
        std::vector<std::shared_ptr<CSaneDevice>> m_devices{};
        std::vector<std::weak_ptr<CSaneDevice>>   m_devices_weak{};
        std::optional<CSaneStatus>                m_initialization_status{};
    };

    inline std::unique_ptr<CSane> g_sane = std::make_unique<CSane>();
}

#endif // !SANE_IN_THE_MEMBRANE
