#ifndef SANE_IN_THE_MEMBRANE_STATUS
#define SANE_IN_THE_MEMBRANE_STATUS

extern "C" {
#include <sane/sane.h>
}

namespace sane_in_the_membrane::sane {
    class CSaneStatus {
      public:
        CSaneStatus(SANE_Status status);

        bool              is_ok() const;
        bool              operator==(const CSaneStatus& other) const;
        bool              operator!=(const CSaneStatus& other) const;
        friend bool       operator==(const SANE_Status lhs, const CSaneStatus& rhs);
        friend bool       operator!=(const SANE_Status lhs, const CSaneStatus& rhs);

        SANE_String_Const str_status() const;

      private:
        SANE_Status m_status;
    };
    bool operator==(const SANE_Status lhs, const CSaneStatus& rhs);
    bool operator!=(const SANE_Status lhs, const CSaneStatus& rhs);
}

#endif // !SANE_IN_THE_MEMBRANE_STATUS
