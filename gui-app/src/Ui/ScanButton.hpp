#ifndef UI_SCAN_BUTTON
#define UI_SCAN_BUTTON

#include "ScannerSelect.hpp"
#include <qobject.h>
#include <qpushbutton.h>
#include <qwidget.h>

namespace sane_in_the_membrane::ui {
    class CScanButton : public QPushButton {
        Q_OBJECT
      public:
        CScanButton(::ui::CScannerSelect* scanner_select, QWidget* parent = nullptr);

      private slots:
        void sl_clicked();

      private:
        ::ui::CScannerSelect* m_scanner_select;
    };
}

#endif // !UI_SCAN_BUTTON
