#ifndef UI_MAIN_WINDOW
#define UI_MAIN_WINDOW

#include <QMainWindow>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <qboxlayout.h>
#include <qevent.h>
#include <qformlayout.h>
#include <qgroupbox.h>
#include <qmessagebox.h>
#include <qobject.h>
#include <qprogressbar.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include "ScannerSelect.hpp"
#include "RefreshScanners.hpp"
#include "ScanButton.hpp"
#include "ImageView.hpp"
#include <QMessageBox>
#include <QProgressBar>

namespace sane_in_the_membrane::ui {

    class CMainWindow : public QMainWindow {
        Q_OBJECT

      public:
        CMainWindow();
        ~CMainWindow();

        void closeEvent(QCloseEvent* event) override; 

      private slots:
        void sl_sig_done(const std::shared_ptr<grpc::Status> status, std::shared_ptr<utils::CFile> file, std::shared_ptr<utils::ScannerParameters> params);

      private:
        QWidget*                                  m_central_widget{};
        QVBoxLayout*                              m_main_layout{};
        QGroupBox*                                m_group_box{};
        QFormLayout*                              m_form_layout{};

        ui::CScannerSelect*                       m_scanner_select{};
        QHBoxLayout*                              m_scanner_hbox{};
        QFormLayout*                              m_scanner_layout{};
        sane_in_the_membrane::ui::CRefreshButton* m_refresh_button{};
        sane_in_the_membrane::ui::CScanButton*    m_scan_button{};
        sane_in_the_membrane::ui::CImageView*     m_image_view{};
        QProgressBar*                             m_progress_bar{};
    };
}

#endif // !#ifndef UI_MAIN_WINDOW
