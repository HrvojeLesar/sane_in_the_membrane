#ifndef UTILS_FILE
#define UTILS_FILE

#include <cstddef>
#include <filesystem>
#include <fstream>

namespace sane_in_the_membrane::utils {
    class CFile {

      public:
        CFile(std::filesystem::path& path);
        CFile(std::filesystem::path&& path);

        CFile(const CFile& other) = delete;
        CFile(CFile&& other);

        std::filesystem::path& path();
        std::ofstream&         output_stream();
        void                   write(const std::string& data);
        void                   write(const std::string&& data);
        size_t                 size() const;

      private:
        std::filesystem::path m_path;
        std::ofstream         m_ofstream;
        size_t                m_size{0};
    };
}

#endif // !UTILS_FILE
