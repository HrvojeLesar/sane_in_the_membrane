#include "MainWindow.hpp"
#include "../Readers/ScanResponseReader.hpp"
#include "../Utils/Globals.hpp"
#include "image/MagickImageWrapper.hpp"
#include <fstream>
#include <iostream>

using namespace sane_in_the_membrane::ui;

CMainWindow::CMainWindow() {
    QObject::connect(&utils::Globals::get()->m_scan_response_reader, &reader::CScanResponseReader::sig_done, this, &CMainWindow::sl_sig_done);

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
    QObject::connect(&utils::Globals::get()->m_scan_response_reader, &sane_in_the_membrane::reader::CScanResponseReader::sig_progress, m_progress_bar,
                     [this](double progress) { this->m_progress_bar->setValue(progress); });

    m_scanner_hbox->addWidget(m_scanner_select);
    m_scanner_hbox->addWidget(m_refresh_button);
    m_scanner_hbox->addWidget(m_progress_bar);

    m_progress_bar->setMinimum(0);
    m_progress_bar->setMaximum(100);

    m_scanner_layout = new QFormLayout();

    m_form_layout->addRow(new QLabel("Scan:"), m_scan_button);
    m_form_layout->addRow(new QLabel("Select:"), m_scanner_hbox);
    // m_form_layout->addRow(new QLabel("Image:"), m_image_view);
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
void CMainWindow::sl_sig_done(const std::shared_ptr<grpc::Status> status, std::shared_ptr<utils::CFile> file, std::shared_ptr<utils::ScannerParameters> params) {
    if (!status->ok()) {
        return;
    }

    if (static_cast<size_t>(params->pixels_per_line * params->lines) > file->size()) {
        std::cout << "File size does not correspond to params\n";
        return;
    }

    std::ifstream     infile(file->path(), std::ios::binary);
    std::vector<char> data{std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>()};
    infile.close();

    auto path = utils::Globals::get()->m_file_manager.generate_temp_filepath(".png");
    sane_in_the_membrane::utils::write_image(path, params->pixels_per_line, params->lines, data.data());
    auto img = new sane_in_the_membrane::ui::CImageView(path);
    m_form_layout->addRow(new QLabel("Images:"), img);
}
