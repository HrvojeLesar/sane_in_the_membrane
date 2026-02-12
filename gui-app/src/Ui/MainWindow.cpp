#include "MainWindow.hpp"
#include "../Utils/Globals.hpp"
#include "ImageView.hpp"

using namespace sane_in_the_membrane::ui;

CMainWindow::CMainWindow() {
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
    QObject::connect(&utils::Globals::get_instance().proxies()->m_scan_response_reader_proxy, &utils::proxy::CScanResponseReaderProxy::sig_progress, m_progress_bar,
                     [this](double progress) { this->m_progress_bar->setValue(progress); });

    m_scanner_hbox->addWidget(m_scanner_select);
    m_scanner_hbox->addWidget(m_refresh_button);
    m_scanner_hbox->addWidget(m_progress_bar);

    m_progress_bar->setMinimum(0);
    m_progress_bar->setMaximum(100);

    m_form_layout->addRow(new QLabel("Scan:"), m_scan_button);
    m_form_layout->addRow(new QLabel("Select:"), m_scanner_hbox);
    m_form_layout->addRow(new QLabel("Pages:"), m_image_view);
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
        case QMessageBox::Close: break;

        case QMessageBox::Cancel:
        default: event->ignore(); break;
    }
}
