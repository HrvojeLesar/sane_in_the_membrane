#include "ScanButton.hpp"
#include "ScannerSelect.hpp"
#include <iostream>
#include <qobject.h>
#include "../Readers/ScanResponseReader.hpp"
#include "../Service/DeviceList.hpp"
#include "../Utils/Globals.hpp"

using namespace sane_in_the_membrane::ui;

CScanButton::CScanButton(ui::CScannerSelect* scanner_select, QWidget* parent) : QPushButton("Scan", parent), m_scanner_select(scanner_select) {
    QObject::connect(this, &CScanButton::clicked, this, &CScanButton::sl_clicked);
    QObject::connect(&utils::Globals::get()->m_scan_response_reader, &reader::CScanResponseReader::sig_done, this, &CScanButton::sl_sig_done);
    QObject::connect(&utils::Globals::get()->m_device_list, &service::CDeviceList::sig_scanners_changed, this, &CScanButton::sl_scanners_changed);
    setDisabled(true);
}

void CScanButton::sl_clicked() {
    setDisabled(true);
    const auto scanner_data = m_scanner_select->currentData();
    std::cout << scanner_data->scanner_name().toStdString() << scanner_data->scanner_display_name().toStdString() << "\n";

    m_request.set_scanner_name(scanner_data->scanner_name().toStdString());
    utils::Globals::get()->m_scan_response_reader.scan(m_request);
}
void CScanButton::sl_sig_done(const std::shared_ptr<grpc::Status> status, std::shared_ptr<utils::CFile> file, std::shared_ptr<utils::ScannerParameters> params) {
    if (!status->ok()) {
        std::cout << "Scan failed\n";
    }
    setDisabled(false);
}

void CScanButton::sl_scanners_changed(const sane_in_the_membrane::utils::SharedAccessGuard<std::vector<std::shared_ptr<service::CScannerItem>>>& items) {
    setDisabled(items->empty());
}
