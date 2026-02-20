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
#include "Scan/ScanWidget.hpp"
#include "ScannerSelect.hpp"
#include "RefreshScanners.hpp"
#include "Scan/ScanButton.hpp"
#include "ImageView.hpp"
#include "SemaphoneWidget.hpp"
#include <QMessageBox>
#include <QProgressBar>

namespace sane_in_the_membrane::ui {

    class CMainWindow : public QMainWindow {
        Q_OBJECT

      public:
        CMainWindow();
        ~CMainWindow();

        void closeEvent(QCloseEvent* event) override;

      private:
        QWidget* const                                  m_central_widget;
        QVBoxLayout* const                              m_main_layout;
        QGroupBox* const                                m_group_box;
        QFormLayout* const                              m_form_layout;

        ui::CScannerSelect* const                       m_scanner_select;
        QHBoxLayout* const                              m_scanner_hbox;
        sane_in_the_membrane::ui::CRefreshButton* const m_refresh_button;
        sane_in_the_membrane::ui::CImageView* const     m_image_view;
        CSemaphoreWidget* const                         m_server_status;
        scan::CScanWidget* const                        m_scan_widget;
    };
}

#endif // !#ifndef UI_MAIN_WINDOW
