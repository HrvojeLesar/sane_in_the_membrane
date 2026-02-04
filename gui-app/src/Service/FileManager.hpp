#ifndef SERVICE_FILE_MANAGER
#define SERVICE_FILE_MANAGER

#include <expected>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "../Utils/File.hpp"

namespace sane_in_the_membrane::service {
    class CFileManager {
      private:
        enum EWriteBehaviour {
            FAIL_ON_DUPLICATE,
            OVERRIDE,
        };

        static const std::expected<std::filesystem::path, std::string> generate_temp_dir();

      public:
        // WARN: Constructor will fail if temp filesystem is not available to the program
        CFileManager();
        CFileManager(std::filesystem::path& temp_dir);
        CFileManager(std::filesystem::path&& temp_dir);

        std::shared_ptr<utils::CFile>            new_temp_file();
        void                                     write_to_file(std::shared_ptr<utils::CFile>& file, std::string& data);
        std::expected<utils::CFile, std::string> write_file(std::filesystem::path path, std::string& data, CFileManager::EWriteBehaviour override);
        bool                                     delete_file(std::shared_ptr<utils::CFile>& file);
        std::filesystem::path                    generate_temp_filepath();
        std::filesystem::path                    generate_temp_filepath(const char* extension);
        std::filesystem::path                    generate_temp_filepath(const std::string& extension);

      private:
        std::filesystem::path                      m_temp_dir;
        std::vector<std::shared_ptr<utils::CFile>> m_managed_temp_files{};
    };
}

#endif // !SERVICE_FILE_MANAGER
