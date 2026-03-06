#include "File.hpp"

using namespace sane_in_the_membrane::utils;

CFile::CFile(std::filesystem::path& path) : m_path(path), m_ofstream(m_path, std::ios::out | std::ios::binary | std::ios::app) {}
CFile::CFile(std::filesystem::path&& path) : m_path(std::move(path)), m_ofstream(m_path, std::ios::out | std::ios::binary | std::ios::app) {}

CFile::CFile(CFile&& other) : m_path(std::move(other.m_path)), m_ofstream(std::move(other.m_ofstream)) {}

const std::filesystem::path& CFile::path() const {
    return m_path;
}

void CFile::write(const std::string& data) {
    m_ofstream.write(data.data(), data.size());
    m_size += data.size();
    m_ofstream.flush();
}

void CFile::write(const std::string&& data) {
    write(data);
}

size_t CFile::size() const {
    return m_size;
}

std::vector<unsigned char> CFile::read() const {
    std::ifstream              infile(path(), std::ios::binary);
    std::vector<unsigned char> data{std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>()};
    infile.close();

    return data;
}

CFileRW::CFileRW(std::filesystem::path& path) : m_path(path) {};
CFileRW::CFileRW(std::filesystem::path&& path) : m_path(path) {};
CFileRW::CFileRW(const CFileRW&& other) : m_path(std::move(other.m_path)) {};

const std::filesystem::path& CFileRW::path() const {
    return m_path;
}

void CFileRW::write(const std::string& data) {
    write(data.data(), data.size());
}

void CFileRW::write(const std::string&& data) {
    write(data);
}

void CFileRW::write(const void* data, std::streamsize data_size) {
    std::ofstream ofstream{m_path, std::ios::out | std::ios::binary};
    ofstream.write(static_cast<const char*>(data), data_size);
    ofstream.flush();
}

void CFileRW::append(const void* data, std::streamsize data_size) {
    std::ofstream ofstream{m_path, std::ios::out | std::ios::binary | std::ios::app};
    ofstream.write(static_cast<const char*>(data), data_size);
    ofstream.flush();
}

std::vector<std::uint8_t> CFileRW::read() const {
    std::ifstream             infile(path(), std::ios::binary);
    std::vector<std::uint8_t> data{std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>()};
    infile.close();

    return data;
}

bool CFileRW::exists() const {
    return std::filesystem::exists(m_path) && std::filesystem::is_regular_file(m_path);
}
