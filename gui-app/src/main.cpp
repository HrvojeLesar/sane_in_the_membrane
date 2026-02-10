#include "Ui/MainApp.hpp"
#include "GlobalLogger.cpp"

int main(int argc, char* argv[]) {
    auto temp_dir_base         = std::filesystem::temp_directory_path();
    auto program_temp_dir_path = temp_dir_base.append("sane_in_the_membrane/app.log");
    auto log_file_result       = g_logger.set_log_file(program_temp_dir_path);
    if (!log_file_result.has_value()) {
        g_logger.log(ERR, log_file_result.error());
    }

    QApplication                       q_app(argc, argv);
    sane_in_the_membrane::ui::CMainApp app(q_app);

    g_logger.log(INFO, "Starting");

    return app.exec();
}
