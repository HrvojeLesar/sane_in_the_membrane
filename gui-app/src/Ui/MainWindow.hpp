#ifndef UI_MAIN_WINDOW
#define UI_MAIN_WINDOW

#include <QMainWindow>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <qboxlayout.h>
#include <qwidget.h>
#include "../scan-ui/scan-button.hpp"
#include "ScannerSelect.hpp"

namespace ui {
    class CMainWindow : public QMainWindow {
      public:
        CMainWindow() : m_central_widget(this), m_main_layout(), m_group_box(), m_form_layout(), m_scan_button(nullptr), m_scanner_select(nullptr) {
            m_form_layout.addRow(new QLabel("Button:"), &m_scan_button);
            m_form_layout.addRow(new QLabel("Select:"), &m_scanner_select);
            m_group_box.setLayout(&m_form_layout);

            m_main_layout.addWidget(&m_group_box);

            m_central_widget.setLayout(&m_main_layout);
            setCentralWidget(&m_central_widget);

            show();
        }

        ~CMainWindow() {}

      private:
        QWidget            m_central_widget;
        QVBoxLayout        m_main_layout;
        QGroupBox          m_group_box;
        QFormLayout        m_form_layout;
        CScanButton        m_scan_button;
        ui::CScannerSelect m_scanner_select;
    };
}

#endif // !#ifndef UI_MAIN_WINDOW
