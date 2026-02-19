#include "GLogger.hpp"

void sane_in_the_membrane::log::log(ELogLevel log_level, const std::string_view& message) {
    g_logger.log(log_level, message);
}

void sane_in_the_membrane::log::trace(const std::string_view& message) {
    g_logger.trace(message);
}

void sane_in_the_membrane::log::debug(const std::string_view& message) {
    g_logger.debug(message);
}

void sane_in_the_membrane::log::info(const std::string_view& message) {
    g_logger.info(message);
}

void sane_in_the_membrane::log::warn(const std::string_view& message) {
    g_logger.warn(message);
}

void sane_in_the_membrane::log::error(const std::string_view& message) {
    g_logger.error(message);
}

void sane_in_the_membrane::log::critical(const std::string_view& message) {
    g_logger.critical(message);
}
