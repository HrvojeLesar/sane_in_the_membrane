#ifndef SANE_IN_THE_MEMBRANE_UTILS_LOGGER
#define SANE_IN_THE_MEMBRANE_UTILS_LOGGER

#include <cstdint>
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
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
        template <typename... Args>
        void log(ELogLevel log_level, const std::format_string<Args...> fmt, Args&&... args) {
            if (log_level < m_level) {
                return;
            }

            std::string message = std::vformat(fmt.get(), std::make_format_args(args...));

            log(log_level, message);
        }

        void trace(const std::string_view& message);
        void debug(const std::string_view& message);
        void info(const std::string_view& message);
        void warn(const std::string_view& message);
        void error(const std::string_view& message);
        void critical(const std::string_view& message);

        template <typename... Args>
        void trace(const std::format_string<Args...> fmt, Args&&... args) {
            log(ELogLevel::LOG_TRACE, fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void debug(const std::format_string<Args...> fmt, Args&&... args) {
            log(ELogLevel::LOG_DEBUG, fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void info(const std::format_string<Args...> fmt, Args&&... args) {
            log(ELogLevel::LOG_INFO, fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void warn(const std::format_string<Args...> fmt, Args&&... args) {
            log(ELogLevel::LOG_WARN, fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void error(const std::format_string<Args...> fmt, Args&&... args) {
            log(ELogLevel::LOG_ERROR, fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void critical(const std::format_string<Args...> fmt, Args&&... args) {
            log(ELogLevel::LOG_CRITICAL, fmt, std::forward<Args>(args)...);
        }

      private:
        ELogLevel     m_level;
        std::mutex    m_log_mutex;

        bool          m_file_enabled{false};
        std::ofstream m_file;
        bool          m_stdout_enabled{true};
        bool          m_enable_coloured_output{true};
    };
}

#endif // !SANE_IN_THE_MEMBRANE_UTILS_LOGGER
