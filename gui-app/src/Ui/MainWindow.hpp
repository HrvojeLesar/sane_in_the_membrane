#ifndef UI_MAIN_WINDOW
#define UI_MAIN_WINDOW

#include <QMainWindow>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <qboxlayout.h>
#include <qformlayout.h>
#include <qgroupbox.h>
#include <qwidget.h>
#include "ScannerSelect.hpp"
#include "RefreshScanners.hpp"

namespace ui {
    class CMainWindow : public QMainWindow {
      public:
        CMainWindow() {

            m_central_widget = new QWidget(this);
            m_main_layout    = new QVBoxLayout();
            m_group_box      = new QGroupBox();
            m_form_layout    = new QFormLayout();
            m_refresh_button = new sane_in_the_membrane::ui::CRefreshButton(nullptr);
            m_scanner_select = new ui::CScannerSelect();

            m_scanner_hbox = new QHBoxLayout();

            m_scanner_hbox->addWidget(m_scanner_select);
            m_scanner_hbox->addWidget(m_refresh_button);

            m_scanner_layout = new QFormLayout();

            // m_form_layout->addRow(new QLabel("Scan:"), m_refresh_button);
            m_form_layout->addRow(new QLabel("Select:"), m_scanner_hbox);
            m_group_box->setLayout(m_form_layout);

            m_main_layout->addWidget(m_group_box);

            m_central_widget->setLayout(m_main_layout);
            setCentralWidget(m_central_widget);

            show();
        }

        ~CMainWindow() {}

      private:
        QWidget*                                  m_central_widget{};
        QVBoxLayout*                              m_main_layout{};
        QGroupBox*                                m_group_box{};
        QFormLayout*                              m_form_layout{};

        ui::CScannerSelect*                       m_scanner_select{};
        QHBoxLayout*                              m_scanner_hbox{};
        QFormLayout*                              m_scanner_layout{};
        sane_in_the_membrane::ui::CRefreshButton* m_refresh_button{};
    };
}

#endif // !#ifndef UI_MAIN_WINDOW
