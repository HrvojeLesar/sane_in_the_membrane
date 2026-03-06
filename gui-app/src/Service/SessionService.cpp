#include "SessionService.hpp"
#include <Assert.hpp>
#include <filesystem>

using namespace sane_in_the_membrane::service;

CSessionService::CSessionService() : m_file_manager(), m_session_file(m_file_manager.new_temp_file_unmanaged<utils::CFileRW>("session")) {}

void CSessionService::save_session(Bytes& data) {
    CSessionHeader header{data.size()};

    m_session_file.write(&header, sizeof(header));
    m_session_file.append(data);
}

std::expected<SSessionData, ESessionLoadError> CSessionService::load_session() {
    if (!m_session_file.exists())
        return std::unexpected(ESessionLoadError::SESSION_FILE_NO_FOUND);

    auto data = m_session_file.read();

    if (data.size() < sizeof(CSessionHeader))
        return std::unexpected(ESessionLoadError::INVALID_HEADER);

    auto header = CSessionHeader::from_raw_bytes(data);
    if (!header.has_value())
        return std::unexpected(ESessionLoadError::HEADER_VERSION_MISMATCH);

    return SSessionData{
        .header = header.value(),
        .data   = Bytes{data.begin() + sizeof(CSessionHeader), data.end()},
    };
}

CSessionHeader::CSessionHeader(std::size_t data_lenght) : m_data_length(data_lenght) {}

std::expected<CSessionHeader, ESessionHeaderConversionError> CSessionHeader::from_raw_bytes(Bytes& data) {
    SITM_ASSERT(data.size() >= sizeof(CSessionHeader), "Session header is not in provided bytes");

    auto raw_header = (CSessionHeader*)data.data();

    if (raw_header->m_version != CSessionHeader::EXPECTED_VERSION)
        return std::unexpected(ESessionHeaderConversionError::HEADER_VERSION_MISMATCH);

    return CSessionHeader{raw_header->m_data_length};
}

void CSessionService::remove_session() {
    std::filesystem::remove(m_session_file.path());
}
