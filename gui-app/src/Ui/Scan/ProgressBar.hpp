#ifndef UI_SCAN_PROGRESS_BAR
#define UI_SCAN_PROGRESS_BAR

#include <qprogressbar.h>
#include <qtmetamacros.h>
#include <qwidget.h>
namespace sane_in_the_membrane::ui::scan {
    class CScanProgress : public QProgressBar {

        Q_OBJECT
      public:
        CScanProgress(QWidget* parent = nullptr);
    };
}

#endif // !UI_SCAN_PROGRESS_BAR
