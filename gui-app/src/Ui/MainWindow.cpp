#include "MainWindow.hpp"
#include "../Utils/Globals.hpp"
#include "ImageView.hpp"
#include "../Utils/mDNS/mDnsAutoFind.hpp"
#include "Scan/ProgressBar.hpp"
#include "SemaphoneWidget.hpp"

using namespace sane_in_the_membrane::ui;

CMainWindow::CMainWindow() :
    m_central_widget(new QWidget(this)), m_main_layout(new QVBoxLayout()), m_group_box(new QGroupBox()), m_form_layout(new QFormLayout()),

    m_scanner_select(new CScannerSelect()), m_scanner_hbox(new QHBoxLayout()), m_refresh_button(new CRefreshButton()), m_image_view(new CImageView()),
    m_server_status(new CSemaphoreWidget()), m_scan_widget(new scan::CScanWidget(m_scanner_select, this)) {
    m_scanner_hbox->addWidget(m_scanner_select);
    m_scanner_hbox->addWidget(m_refresh_button);

    m_form_layout->addRow(new QLabel("Scan:"), m_scan_widget);
    m_form_layout->addRow(new QLabel("Select:"), m_scanner_hbox);
    m_form_layout->addRow(new QLabel("Pages:"), m_image_view);
    m_form_layout->addRow(new QLabel(), m_server_status);
    m_group_box->setLayout(m_form_layout);

    m_main_layout->addWidget(m_group_box);

    m_central_widget->setLayout(m_main_layout);
    setCentralWidget(m_central_widget);

    show();
}
CMainWindow::~CMainWindow() {}

void CMainWindow::closeEvent(QCloseEvent* event) {
    QMessageBox confirm_box{};
    confirm_box.setText("Close application ?");

    confirm_box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    confirm_box.setDefaultButton(QMessageBox::Ok);

    auto exec_status = confirm_box.exec();

    switch (exec_status) {
        case QMessageBox::Ok:
        case QMessageBox::Close:
            sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::get_instance().interrupt();
            event->accept();
            break;

        case QMessageBox::Cancel:
        default: event->ignore(); break;
    }
}
