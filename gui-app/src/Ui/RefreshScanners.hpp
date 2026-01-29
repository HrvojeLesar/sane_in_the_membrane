#ifndef UI_REFRESH_SCANNERS
#define UI_REFRESH_SCANNERS

#include "scanner/v1/scanner.pb.h"
#include <qobject.h>
#include <qpushbutton.h>
#include <qwidget.h>

namespace sane_in_the_membrane::ui {
    class CRefreshButton : public QPushButton {
      public:
        CRefreshButton(QWidget* parent = nullptr);

      private slots:
        void sl_refresh_scanners_failed();
        void sl_refresh_scanners();
        void sl_get_scanners_failed();
        void sl_get_scanners(std::shared_ptr<scanner::v1::GetScannersResponse> response);

      private:
        void on_clicked_get_scanners();
        void get_scanners();
        void refresh_scanners();
        void enable_button();
        void refreshing();
    };
}

#endif // !UI_REFRESH_SCANNERS
