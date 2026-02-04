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
#include <qwidget.h>
#include "ScannerSelect.hpp"
#include "RefreshScanners.hpp"
#include "ScanButton.hpp"
#include "ImageView.hpp"
#include <QMessageBox>
#include <QProgressBar>

namespace sane_in_the_membrane::ui {

    class CMainWindow : public QMainWindow {
      public:
        CMainWindow() {

            m_central_widget = new QWidget(this);
            m_main_layout    = new QVBoxLayout();
            m_group_box      = new QGroupBox();
            m_form_layout    = new QFormLayout();
            m_refresh_button = new sane_in_the_membrane::ui::CRefreshButton(nullptr);
            m_scanner_select = new ui::CScannerSelect();
            m_image_view     = new sane_in_the_membrane::ui::CImageView();
            m_progress_bar   = new QProgressBar();

            m_scanner_hbox = new QHBoxLayout();

            m_scan_button = new sane_in_the_membrane::ui::CScanButton(m_scanner_select);

            m_scanner_hbox->addWidget(m_scanner_select);
            m_scanner_hbox->addWidget(m_refresh_button);
            m_scanner_hbox->addWidget(m_progress_bar);

            m_progress_bar->setMinimum(0);
            m_progress_bar->setMaximum(100);

            m_scanner_layout = new QFormLayout();

            m_form_layout->addRow(new QLabel("Scan:"), m_scan_button);
            m_form_layout->addRow(new QLabel("Select:"), m_scanner_hbox);
            m_form_layout->addRow(new QLabel("Image:"), m_image_view);
            m_group_box->setLayout(m_form_layout);

            m_main_layout->addWidget(m_group_box);

            m_central_widget->setLayout(m_main_layout);
            setCentralWidget(m_central_widget);

            show();
        }

        ~CMainWindow() {}

        void closeEvent(QCloseEvent* event) override {
            QMessageBox confirm_box{};
            confirm_box.setText("Close application ?");

            confirm_box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            confirm_box.setDefaultButton(QMessageBox::Ok);

            auto exec_status = confirm_box.exec();

            switch (exec_status) {
                case QMessageBox::Ok:
                case QMessageBox::Close: break;

                case QMessageBox::Cancel:
                default: event->ignore(); break;
            }
        }

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
