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
    std::string                 prefix_coloured{""};
    switch (log_level) {
        case LOG_TRACE:
            prefix          = "TRACE: ";
            prefix_coloured = "\033[36mTRACE: \033[0m";
            break;
        case LOG_DEBUG:
            prefix          = "DEBUG: ";
            prefix_coloured = "\033[32mDEBUG: \033[0m";
            break;
        case LOG_INFO:
            prefix          = "INFO: ";
            prefix_coloured = "\033[34mINFO: \033[0m";
            break;
        case LOG_WARN:
            prefix          = "WARN: ";
            prefix_coloured = "\033[33mWARN: \033[0m";
            break;
        case LOG_ERROR:
            prefix          = "ERROR: ";
            prefix_coloured = "\033[31mERROR: \033[0m";
            break;
        case LOG_CRITICAL:
            prefix          = "CRITICAL: ";
            prefix_coloured = "\033[35mCRITICAL: \033[0m";
            break;
    }

    std::cout << (!m_enable_coloured_output ? prefix : prefix_coloured) << message << "\n";
    std::cout.flush();

    if (m_file_enabled) {
        m_file << prefix << message << "\n";
    }
}

void CLogger::trace(const std::string_view& message) {
    log(ELogLevel::LOG_TRACE, message);
}

void CLogger::debug(const std::string_view& message) {
    log(ELogLevel::LOG_DEBUG, message);
}

void CLogger::info(const std::string_view& message) {
    log(ELogLevel::LOG_INFO, message);
}

void CLogger::warn(const std::string_view& message) {
    log(ELogLevel::LOG_WARN, message);
}

void CLogger::error(const std::string_view& message) {
    log(ELogLevel::LOG_ERROR, message);
}

void CLogger::critical(const std::string_view& message) {
    log(ELogLevel::LOG_CRITICAL, message);
}
