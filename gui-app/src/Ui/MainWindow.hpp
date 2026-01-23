#ifndef UI_MAIN_WINDOW
#define UI_MAIN_WINDOW

#include <qmainwindow.h>
#include <qpushbutton.h>
#include "../scan-ui/scan-button.hpp"

namespace ui {
    class CMainWindow : public QMainWindow {
      public:
        CMainWindow(scanner::v1::ScannerService::Stub& stub) : m_stub(stub), m_scan_button(this, m_stub) {
            show();
        }
        ~CMainWindow() {}

      private:
        scanner::v1::ScannerService::Stub& m_stub;
        CScanButton                        m_scan_button;
    };
}

#endif // !#ifndef UI_MAIN_WINDOW
