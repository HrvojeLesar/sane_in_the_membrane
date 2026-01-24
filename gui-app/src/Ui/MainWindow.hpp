#ifndef UI_MAIN_WINDOW
#define UI_MAIN_WINDOW

#include <QMainWindow>
#include "../scan-ui/scan-button.hpp"

namespace ui {
    class CMainWindow : public QMainWindow {
      public:
        CMainWindow() : m_scan_button(this) {
            show();
        }

        ~CMainWindow() {}

      private:
        CScanButton m_scan_button;
    };
}

#endif // !#ifndef UI_MAIN_WINDOW
