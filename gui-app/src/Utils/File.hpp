#ifndef UTILS_FILE
#define UTILS_FILE

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ios>
#include <qobject.h>
#include <vector>
#include <cstdint>

namespace sane_in_the_membrane::utils {
    class CFile {
      public:
        CFile(std::filesystem::path& path);
        CFile(std::filesystem::path&& path);

        CFile(const CFile& other) = delete;
        CFile(CFile&& other);
        virtual ~CFile() = default;

        const std::filesystem::path& path() const;
        void                         write(const std::string& data);
        void                         write(const std::string&& data);
        size_t                       size() const;
        std::vector<unsigned char>   read() const;

      private:
        std::filesystem::path m_path;
        std::ofstream         m_ofstream;
        size_t                m_size{0};
    };

    class CFileRW {
      public:
        CFileRW(std::filesystem::path& path);
        CFileRW(std::filesystem::path&& path);
        CFileRW(const CFileRW&& other);

        const std::filesystem::path& path() const;
        void                         write(const std::string& data);
        void                         write(const std::string&& data);
        void                         write(const void* data, std::streamsize data_size);

        template <typename VectorLike>
        void append(const VectorLike& data) {
            append(data.data(), data.size());
        }
        void                      append(const void* data, std::streamsize data_size);
        std::vector<std::uint8_t> read() const;
        bool                      exists() const;

      private:
        std::filesystem::path m_path;
    };
}

#endif // !UTILS_FILE
