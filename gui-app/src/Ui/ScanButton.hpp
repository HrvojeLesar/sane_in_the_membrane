#ifndef UI_SCAN_BUTTON
#define UI_SCAN_BUTTON

#include "ScannerSelect.hpp"
#include "scanner/v1/scanner.pb.h"
#include <grpcpp/support/status.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include "../Utils/File.hpp"
#include "../Utils/ScannerUtils.hpp"

namespace sane_in_the_membrane::ui {
    class CScanButton : public QPushButton {
        Q_OBJECT
      public:
        CScanButton(ui::CScannerSelect* scanner_select, QWidget* parent = nullptr);

      private slots:
        void sl_clicked();
        void sl_sig_done(const std::shared_ptr<grpc::Status> status, std::shared_ptr<utils::CFile> file, std::shared_ptr<utils::ScannerParameters> params);
        void sl_scanners_changed(const sane_in_the_membrane::utils::SharedAccessGuard<std::vector<std::shared_ptr<service::CScannerItem>>>& items);

      private:
        ui::CScannerSelect*      m_scanner_select;
        scanner::v1::ScanRequest m_request;
    };
}

#endif // !UI_SCAN_BUTTON
