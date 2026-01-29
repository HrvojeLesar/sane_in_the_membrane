#include "Ui/MainApp.hpp"

int main(int argc, char* argv[]) {
    ui::args.SetMaxReceiveMessageSize(50 * 1024 * 1024);
    ui::CMainApp app(argc, argv);
    return app.exec();
}
