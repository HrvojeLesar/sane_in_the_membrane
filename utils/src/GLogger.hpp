#ifndef SANE_IN_THE_MEMBRANE_UTILS_GLOGGER
#define SANE_IN_THE_MEMBRANE_UTILS_GLOGGER

#include "Logger.hpp"
namespace sane_in_the_membrane::log {

    using namespace sane_in_the_membrane::utils;

    inline static utils::CLogger g_logger{};

    void                         log(ELogLevel log_level, const std::string_view& message);
    template <typename... Args>
    void log(ELogLevel log_level, const std::format_string<Args...> fmt, Args&&... args) {
        g_logger.log(log_level, fmt, args...);
    }

    template <typename... Args>
    void trace(const std::format_string<Args...> fmt, Args&&... args) {
        g_logger.trace(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void debug(const std::format_string<Args...> fmt, Args&&... args) {
        g_logger.debug(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void info(const std::format_string<Args...> fmt, Args&&... args) {
        g_logger.info(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warn(const std::format_string<Args...> fmt, Args&&... args) {
        g_logger.warn(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(const std::format_string<Args...> fmt, Args&&... args) {
        g_logger.error(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void critical(const std::format_string<Args...> fmt, Args&&... args) {
        g_logger.critical(fmt, std::forward<Args>(args)...);
    }

    void trace(const std::string_view& message);
    void debug(const std::string_view& message);
    void info(const std::string_view& message);
    void warn(const std::string_view& message);
    void error(const std::string_view& message);
    void critical(const std::string_view& message);
}

#endif // !SANE_IN_THE_MEMBRANE_UTILS_GLOGGER
