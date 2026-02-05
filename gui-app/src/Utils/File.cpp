#include "File.hpp"
#include <fstream>
#include <ios>

using namespace sane_in_the_membrane::utils;

CFile::CFile(std::filesystem::path& path) : m_path(path), m_ofstream(m_path, std::ios::out | std::ios::binary) {}
CFile::CFile(std::filesystem::path&& path) : m_path(std::move(path)), m_ofstream(m_path, std::ios::out | std::ios::binary) {}

CFile::CFile(CFile&& other) : m_path(std::move(other.m_path)), m_ofstream(std::move(other.m_ofstream)) {}

const std::filesystem::path& CFile::path() const {
    return m_path;
}

const std::ofstream& CFile::output_stream() const {
    return m_ofstream;
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
