#ifndef UI_SELECT_SCANNER_SELECT_WIDGET
#define UI_SELECT_SCANNER_SELECT_WIDGET

#include "RefreshScanners.hpp"
#include "ScannerSelect.hpp"
#include <qboxlayout.h>
#include <qtmetamacros.h>
#include <qwidget.h>
namespace sane_in_the_membrane::ui::select {

    class CScannerSelectWidget : public QWidget {
        Q_OBJECT

      public:
        CScannerSelectWidget(QWidget* parent = nullptr);

        CScannerSelect* get_scanner_select();

      private:
        QHBoxLayout* const    m_layout;
        CRefreshButton* const m_refresh_button;
        CScannerSelect* const m_scanner_select;
    };
}

#endif // !UI_SELECT_SCANNER_SELECT_WIDGET
