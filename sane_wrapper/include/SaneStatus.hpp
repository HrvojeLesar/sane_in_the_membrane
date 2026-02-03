#ifndef SANE_IN_THE_MEMBRANE_STATUS
#define SANE_IN_THE_MEMBRANE_STATUS

extern "C" {
#include <sane/sane.h>
}

namespace sane_in_the_membrane::sane {
    class CSaneStatus {
      public:
        CSaneStatus(SANE_Status status);

        bool is_ok() const;
        bool operator==(const CSaneStatus& other) const;
        bool operator!=(const CSaneStatus& other) const;
        bool operator==(const SANE_Status& other) const;
        bool operator!=(const SANE_Status& other) const;

        SANE_String_Const str_status() const;

      private:
        SANE_Status m_status;
    };
}

#endif // !SANE_IN_THE_MEMBRANE_STATUS
