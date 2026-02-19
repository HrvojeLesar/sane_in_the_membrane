#include "Ui/MainApp.hpp"
#include <GLogger.hpp>

#ifdef _WIN32
#include "windows.hpp"
#endif

int main(int argc, char* argv[]) {
    auto temp_dir_base         = std::filesystem::temp_directory_path();
    auto program_temp_dir_path = temp_dir_base.append("sane_in_the_membrane/app.log");
    auto log_file_result       = sane_in_the_membrane::log::g_logger.set_log_file(program_temp_dir_path);
    if (!log_file_result.has_value()) {
        sane_in_the_membrane::log::error(log_file_result.error());
    }

    QApplication                       q_app(argc, argv);
    sane_in_the_membrane::ui::CMainApp app(q_app);

    sane_in_the_membrane::log::info("Starting");

    return app.exec();
}
