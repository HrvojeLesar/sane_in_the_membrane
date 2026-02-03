#include "Ui/MainApp.hpp"

int main(int argc, char* argv[]) {
    QApplication                       q_app(argc, argv);
    sane_in_the_membrane::ui::CMainApp app(q_app);
    return app.exec();
}
