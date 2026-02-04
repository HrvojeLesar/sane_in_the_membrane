#include "File.hpp"

using namespace sane_in_the_membrane::utils;

CFile::CFile(std::filesystem::path& path) : m_path(path), m_ofstream(m_path, std::ios::out | std::ios::binary) {}
CFile::CFile(std::filesystem::path&& path) : m_path(std::move(path)), m_ofstream(m_path, std::ios::out | std::ios::binary) {}

CFile::CFile(CFile&& other) : m_path(std::move(other.m_path)), m_ofstream(std::move(other.m_ofstream)) {}

std::filesystem::path& CFile::path() {
    return m_path;
}

std::ofstream& CFile::output_stream() {
    return m_ofstream;
}

void CFile::write(const std::string& data) {
    m_ofstream.write(data.data(), data.size());
    m_ofstream.flush();
}

void CFile::write(const std::string&& data) {
    write(data);
}
