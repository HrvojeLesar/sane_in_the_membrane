#ifndef SANE_IN_THE_MEMBRANE_UTILS_LOGGER
#define SANE_IN_THE_MEMBRANE_UTILS_LOGGER

#include <cstdint>
#include <expected>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <string_view>
namespace sane_in_the_membrane::utils {

    enum ELogLevel : uint8_t {
        LOG_TRACE = 0,
        LOG_DEBUG,
        LOG_INFO,
        LOG_WARN,
        LOG_ERROR,
        LOG_CRITICAL,
    };

    class CLogger {

      public:
        CLogger(ELogLevel log_level = ELogLevel::LOG_DEBUG);
        ~CLogger();

        CLogger(CLogger&)  = delete;
        CLogger(CLogger&&) = delete;

        void                             set_log_level(ELogLevel log_level);
        void                             set_stdout_enabled(bool enabled);
        std::expected<void, std::string> set_log_file(const std::filesystem::path& file);
        void                             log(ELogLevel log_level, const std::string_view& message);

      private:
        ELogLevel     m_level;
        std::mutex    m_log_mutex;

        bool          m_file_enabled{false};
        std::ofstream m_file;
        bool          m_stdout_enabled{true};
    };
}

#endif // !SANE_IN_THE_MEMBRANE_UTILS_LOGGER
