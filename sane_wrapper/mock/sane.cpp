#include <sane/sane.h>

extern "C" {
namespace sane_in_the_membrane::mock {
    struct SSaneHandle {
        SANE_Option_Descriptor option_descriptor{};
    };

    static inline const SSaneHandle g_sane_handle{};
#define TEST_SANE_HANDLE (void*)&g_sane_handle

    SANE_Status sane_init(SANE_Int* version_code, SANE_Auth_Callback authorize) {
        return SANE_STATUS_GOOD;
    }

    void        sane_exit(void) {}

    SANE_Status sane_get_devices(const SANE_Device*** device_list, SANE_Bool local_only) {
        static const SANE_Device mock_device_1 = {.name = "MOCK_NAME_1", .vendor = "MOCK_VENDOR_1", .model = "MOCK_MODEL_1", .type = "MOCK_SCANNER_TYPE_1"};

        auto                     devices = new const SANE_Device* [] { &mock_device_1, nullptr };
        *device_list                     = devices;

        return SANE_STATUS_GOOD;
    }

    SANE_Status sane_open(SANE_String_Const devicename, SANE_Handle* handle) {

        *handle = TEST_SANE_HANDLE;

        return SANE_STATUS_GOOD;
    }

    void                          sane_close(SANE_Handle handle) {}

    const SANE_Option_Descriptor* sane_get_option_descriptor(SANE_Handle handle, SANE_Int option) {
        if (handle != TEST_SANE_HANDLE)
            return nullptr;

        return &g_sane_handle.option_descriptor;
    }

    SANE_Status sane_control_option(SANE_Handle handle, SANE_Int option, SANE_Action action, void* value, SANE_Int* info) {
        return SANE_STATUS_GOOD;
    }

    SANE_Status sane_get_parameters(SANE_Handle handle, SANE_Parameters* params) {
        return SANE_STATUS_GOOD;
    }

    SANE_Status sane_start(SANE_Handle handle) {
        return SANE_STATUS_GOOD;
    }

    SANE_Status sane_read(SANE_Handle handle, SANE_Byte* data, SANE_Int max_length, SANE_Int* length) {
        return SANE_STATUS_GOOD;
    }

    void        sane_cancel(SANE_Handle handle) {}

    SANE_Status sane_set_io_mode(SANE_Handle handle, SANE_Bool non_blocking) {
        return SANE_STATUS_GOOD;
    }

    SANE_Status sane_get_select_fd(SANE_Handle handle, SANE_Int* fd) {
        return SANE_STATUS_GOOD;
    }

    SANE_String_Const sane_strstatus(SANE_Status status) {
        switch (status) {
            case SANE_STATUS_GOOD: return "GOOD";
            case SANE_STATUS_UNSUPPORTED: return "UNSUPPORTED";
            case SANE_STATUS_CANCELLED: return "CANCELLED";
            case SANE_STATUS_DEVICE_BUSY: return "DEVICE_BUSY";
            case SANE_STATUS_INVAL: return "INVAL";
            case SANE_STATUS_EOF: return "EOF";
            case SANE_STATUS_JAMMED: return "JAMMED";
            case SANE_STATUS_NO_DOCS: return "NO_DOCS";
            case SANE_STATUS_COVER_OPEN: return "COVER_OPEN";
            case SANE_STATUS_IO_ERROR: return "IO_ERROR";
            case SANE_STATUS_NO_MEM: return "NO_MEM";
            case SANE_STATUS_ACCESS_DENIED: return "ACCESS_DENIED";
            default: return "UNKNOWN";
        }
    }
}
}
