#include "scan-ui/scan-button.hpp"
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QMainWindow>
#include <qpushbutton.h>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QMainWindow  window;
    CScanButton  button{&window};

    window.show();
    QApplication::exec();

    return 0;
}
