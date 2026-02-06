#include "Logger.hpp"
#include <iostream>
#include <mutex>

using namespace sane_in_the_membrane::utils;

CLogger::CLogger(ELogLevel log_level) : m_level(log_level), m_file{} {}
CLogger::~CLogger() {}

void CLogger::set_log_level(ELogLevel log_level) {
    m_level = log_level;
}

void CLogger::set_log_file(const std::filesystem::path& file) {
    m_file_enabled = true;
    m_file         = std::ofstream{file, std::ios::trunc};
}

void CLogger::set_stdout_enabled(bool enabled) {
    m_stdout_enabled = enabled;
}

void CLogger::log(ELogLevel log_level, const std::string_view& message) {
    if (log_level < m_level) {
        return;
    }

    std::lock_guard<std::mutex> guard{m_log_mutex};

    std::string                 prefix{""};
    switch (log_level) {
        case LOG_TRACE: prefix = "TRACE: "; break;
        case LOG_DEBUG: prefix = "DEBUG: "; break;
        case LOG_INFO: prefix = "INFO: "; break;
        case LOG_WARN: prefix = "WARN"; break;
        case LOG_ERROR: prefix = "ERROR"; break;
        case LOG_CRITICAL: prefix = "CRITICAL"; break;
    }

    std::cout << prefix << message;
    std::cout.flush();

    if (m_file_enabled) {
        m_file << prefix << message << "\n";
    }
}
