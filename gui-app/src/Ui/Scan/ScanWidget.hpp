#ifndef UI_SCAN_SCAN_WIDGET
#define UI_SCAN_SCAN_WIDGET

#include "ProgressBar.hpp"
#include "ScanButton.hpp"
#include <qboxlayout.h>
#include <qtmetamacros.h>
#include <qwidget.h>

namespace sane_in_the_membrane::ui::scan {

    class CScanWidget : public QWidget {
        Q_OBJECT

      public:
        CScanWidget(ui::CScannerSelect* scanner_select, QWidget* parent = nullptr);

      private:
        CScanButton* const   m_scan_button;
        CScanProgress* const m_scan_progress;
        QHBoxLayout* const   m_layout;
    };

}

#endif // !UI_SCAN_SCAN_WIDGET
