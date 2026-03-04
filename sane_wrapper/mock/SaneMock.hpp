#include <memory>
#include <sane/sane.h>
#include <string>
#include <vector>
#include <cstddef>

namespace sane_in_the_membrane::mock {
    struct SMockDevice {
        SMockDevice();
        SMockDevice(size_t number);

        SANE_Device device;
        std::string name;
        std::string vendor;
        std::string model;
        std::string type;
    };

    class CMockHandle {
      public:
        CMockHandle(SMockDevice* device);
        CMockHandle(const CMockHandle& handle, const SMockDevice* device);

        static inline size_t MAX_READ_LENGTH{1024};

        void                 set_max_read_bytes(size_t max_length);
        size_t               get_max_read_bytes();
        size_t               get_read_bytes_capacity();
        size_t               get_bytes_read();
        void                 set_bytes_read(size_t bytes);

      public:
        SANE_Option_Descriptor   m_option_descriptor{};
        bool                     m_open{false};
        const SMockDevice* const m_device;
        SANE_Parameters          m_parameters{.format = SANE_FRAME_RED, .last_frame = true, .bytes_per_line = 1024, .pixels_per_line = 1024, .lines = 1024, .depth = 8};
        bool                     m_started{false};
        bool                     m_cancelled{false};

      private:
        size_t m_max_read_length{MAX_READ_LENGTH};
        size_t m_bytes_read{0};
    };

    class CSaneState {

      public:
        CSaneState();
        ~CSaneState();
        void                add_device(const SMockDevice& device);
        void                add_handle(const CMockHandle& mock_handle);
        void                cleanup_device_list();
        const SANE_Device** sane_comptabile_device_list();
        void                generate_mock_device();
        CMockHandle*        get_handle(const std::string& device_name) const;
        CMockHandle*        get_handle(const SANE_Handle handle) const;
        CMockHandle*        get_or_create_handle(const std::string& device_name);

      private:
        const SANE_Device** m_sane_compatible_device_list{nullptr};

      public:
        bool                                      m_initialized{false};
        std::vector<std::unique_ptr<SMockDevice>> m_devices{};
        std::vector<std::unique_ptr<CMockHandle>> m_handles{};

      public:
        static CSaneState& instance();
    };
}
