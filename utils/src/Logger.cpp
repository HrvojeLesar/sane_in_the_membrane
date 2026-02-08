#include "Logger.hpp"
#include <filesystem>
#include <iostream>
#include <mutex>

using namespace sane_in_the_membrane::utils;

CLogger::CLogger(ELogLevel log_level) : m_level(log_level), m_file{} {}
CLogger::~CLogger() {}

void CLogger::set_log_level(ELogLevel log_level) {
    m_level = log_level;
}

std::expected<void, std::string> CLogger::set_log_file(const std::filesystem::path& filepath) {
    if (filepath.empty()) {
        m_file_enabled = false;
        m_file         = {};
        return {};
    }

    auto parent_directory = filepath.parent_path();
    if (!std::filesystem::exists(parent_directory))
        std::filesystem::create_directories(parent_directory);

    if (!std::filesystem::exists(parent_directory))
        return std::unexpected("Failed to access log file");

    m_file_enabled = true;
    m_file         = std::ofstream{filepath, std::ios::trunc};

    if (!m_file.good())
        return std::unexpected("Failed to open write stream to log file");

    return {};
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
        case LOG_WARN: prefix = "WARN: "; break;
        case LOG_ERROR: prefix = "ERROR: "; break;
        case LOG_CRITICAL: prefix = "CRITICAL: "; break;
    }

    std::cout << prefix << message << "\n";
    std::cout.flush();

    if (m_file_enabled) {
        m_file << prefix << message << "\n";
    }
}
