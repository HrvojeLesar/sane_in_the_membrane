#include "FileManager.hpp"
#include "UniqueIdGenerator.hpp"
#include <expected>
#include <filesystem>
#include <format>
#include <memory>
#include <string>
#include "../Utils/File.hpp"

using namespace sane_in_the_membrane::service;

CFileManager::CFileManager() : m_temp_dir(CFileManager::generate_temp_dir().value()) {}
CFileManager::CFileManager(std::filesystem::path& temp_dir) : m_temp_dir(temp_dir) {}
CFileManager::CFileManager(std::filesystem::path&& temp_dir) : m_temp_dir(std::move(temp_dir)) {}

std::expected<sane_in_the_membrane::utils::CFile, std::string> CFileManager::write_file(std::filesystem::path path, std::string& data, CFileManager::EWriteBehaviour override) {
    if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path) && override != CFileManager::EWriteBehaviour::OVERRIDE) {
        return std::unexpected<std::string>("File exists");
    }

    utils::CFile file{path};
    file.write(data);

    return file;
}

std::shared_ptr<sane_in_the_membrane::utils::CFile> CFileManager::new_temp_file() {
    m_managed_temp_files.emplace_back(std::make_shared<sane_in_the_membrane::utils::CFile>(generate_temp_filepath()));

    return m_managed_temp_files.back();
}

std::shared_ptr<sane_in_the_membrane::utils::CFile> CFileManager::new_temp_file_with_extension(const char* extension) {
    m_managed_temp_files.emplace_back(std::make_shared<sane_in_the_membrane::utils::CFile>(generate_temp_filepath(extension)));

    return m_managed_temp_files.back();
}

std::shared_ptr<sane_in_the_membrane::utils::CFile> CFileManager::new_temp_file_with_extension(const std::string& extension) {
    m_managed_temp_files.emplace_back(std::make_shared<sane_in_the_membrane::utils::CFile>(generate_temp_filepath(extension)));

    return m_managed_temp_files.back();
}

void CFileManager::write_to_file(std::shared_ptr<utils::CFile>& file, std::string& data) {
    file->write(data);
}

bool CFileManager::delete_file(const std::shared_ptr<utils::CFile>& file) {
    if (!std::filesystem::exists(file->path()) || !std::filesystem::is_regular_file(file->path())) {
        return false;
    }

    auto is_removed = std::filesystem::remove(file->path());
    if (!is_removed) {
        return false;
    }

    m_managed_temp_files.erase(std::remove(m_managed_temp_files.begin(), m_managed_temp_files.end(), file), m_managed_temp_files.end());

    return true;
}

const std::expected<std::filesystem::path, std::string> CFileManager::generate_temp_dir() {
    auto temp_dir_base         = std::filesystem::temp_directory_path();
    auto program_temp_dir_path = temp_dir_base.append("sane_in_the_membrane");

    if (std::filesystem::exists(program_temp_dir_path) && std::filesystem::is_directory(program_temp_dir_path)) {
        return program_temp_dir_path;
    }

    if (!std::filesystem::create_directory(program_temp_dir_path)) {
        return std::unexpected<std::string>("Failed to create directory");
    }

    return program_temp_dir_path;
}
std::filesystem::path CFileManager::generate_temp_filepath() {
    auto id       = CUniqueIdGenerator::id();
    auto filepath = m_temp_dir;
    filepath.append(std::format("{}", id));

    return filepath;
}

std::filesystem::path CFileManager::generate_temp_filepath(const char* extension) {
    return generate_temp_filepath().replace_extension(extension);
}

std::filesystem::path CFileManager::generate_temp_filepath(const std::string& extension) {
    return generate_temp_filepath(extension.data());
}
