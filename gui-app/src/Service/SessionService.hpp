#ifndef SERVICE_SESSION_SERVICE
#define SERVICE_SESSION_SERVICE

#include <cstdint>
#include <expected>
#include <vector>
#include "version.hpp"
#include "FileManager.hpp"
#include "Utils/File.hpp"

namespace sane_in_the_membrane::service {
    using Bytes = std::vector<std::uint8_t>;

    enum class ESessionLoadError {
        SESSION_FILE_NO_FOUND,
        DECODING_FAILED,
        INVALID_HEADER,
        HEADER_VERSION_MISMATCH,
    };

    enum class ESessionHeaderConversionError {
        HEADER_VERSION_MISMATCH,
    };

    struct SSessionData;

    class CSessionService {
      public:
        CSessionService();
        void                                           save_session(Bytes& data);
        std::expected<SSessionData, ESessionLoadError> load_session();

      private:
        CFileManager   m_file_manager;
        utils::CFileRW m_session_file;
    };

    class CSessionHeader {
        static constexpr std::size_t EXPECTED_VERSION{VERSION_MINOR};

      public:
        CSessionHeader(std::size_t data_lenght);

        static std::expected<CSessionHeader, ESessionHeaderConversionError> from_raw_bytes(Bytes& data);

      private:
        std::size_t m_version{VERSION_MINOR};
        std::size_t m_data_length;
    };

    struct SSessionData {
        CSessionHeader header;
        Bytes          data;
    };

}

#endif // !SERVICE_SESSION_SERVICE
