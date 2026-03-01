#include "MainWindow.hpp"
#include "ImageView.hpp"
#include "../Utils/mDNS/mDnsAutoFind.hpp"
#include "Select/ScannerSelectWidget.hpp"
#include "SemaphoreWidget.hpp"
#include <qformlayout.h>

using namespace sane_in_the_membrane::ui;

CMainWindow::CMainWindow() :
    m_central_widget(new QWidget(this)), m_main_layout(new QVBoxLayout()), m_group_box(new QGroupBox()), m_image_view(new CImageView()),
    m_scanner_select_widget(new select::CScannerSelectWidget()), m_server_status(new CSemaphoreWidget()),
    m_scan_widget(new scan::CScanWidget(m_scanner_select_widget->get_scanner_select(), this)), m_scanner_form_layout(new QFormLayout(this)),
    m_scanner_section_group(new QGroupBox("Scanners")), m_images_form_layout(new QFormLayout(this)), m_images_section_group(new QGroupBox("Scanned pages")),
    m_other_form_layout(new QFormLayout(this))

{
    m_scanner_form_layout->addRow(nullptr, m_scan_widget);
    m_scanner_form_layout->addRow(nullptr, m_scanner_select_widget);
    m_scanner_section_group->setLayout(m_scanner_form_layout);

    m_images_form_layout->addRow(nullptr, m_image_view);
    m_images_section_group->setLayout(m_images_form_layout);

    m_other_form_layout->addRow(nullptr, m_server_status);

    m_main_layout->addWidget(m_scanner_section_group);
    m_main_layout->addWidget(m_images_section_group);
    m_main_layout->addLayout(m_other_form_layout);

    m_central_widget->setLayout(m_main_layout);
    setCentralWidget(m_central_widget);

    setWindowTitle("Scanners");

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
