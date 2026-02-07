#include "Ui/MainApp.hpp"
#include "GlobalLogger.cpp"

int main(int argc, char* argv[]) {
    auto temp_dir_base         = std::filesystem::temp_directory_path();
    auto program_temp_dir_path = temp_dir_base.append("sane_in_the_membrane/app.log");
    g_logger->set_log_file(program_temp_dir_path);

    QApplication                       q_app(argc, argv);
    sane_in_the_membrane::ui::CMainApp app(q_app);
    return app.exec();
}
