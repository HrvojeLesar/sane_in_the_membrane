#include "Ui/MainApp.hpp"

int main(int argc, char* argv[]) {
    QApplication q_app(argc, argv);
    ui::CMainApp app(q_app);
    return app.exec();
}
