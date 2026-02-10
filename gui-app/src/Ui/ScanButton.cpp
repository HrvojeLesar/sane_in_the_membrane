#include "ScanButton.hpp"
#include "ScannerSelect.hpp"
#include <format>
#include <qobject.h>
#include "../Service/DeviceList.hpp"
#include "../Utils/Globals.hpp"
#include "../GlobalLogger.cpp"

using namespace sane_in_the_membrane::ui;

CScanButton::CScanButton(ui::CScannerSelect* scanner_select, QWidget* parent) : QPushButton("Scan", parent), m_scanner_select(scanner_select) {
    QObject::connect(this, &CScanButton::clicked, this, &CScanButton::sl_clicked);
    QObject::connect(&utils::Globals::get()->m_scan_response_reader_proxy, &utils::proxy::CScanResponseReaderProxy::sig_done, this, &CScanButton::sl_sig_done);
    QObject::connect(&utils::Globals::get()->m_device_list_proxy, &utils::proxy::CDeviceListProxy::sig_scanners_changed, this, &CScanButton::sl_scanners_changed);
    setDisabled(true);
}

void CScanButton::sl_clicked() {
    setDisabled(true);
    const auto scanner_data = m_scanner_select->currentData();
    g_logger.log(TRACE, std::format("Scanning with: {} {}", scanner_data->scanner_name().toStdString(), scanner_data->scanner_display_name().toStdString()));

    m_request.set_scanner_name(scanner_data->scanner_name().toStdString());
    utils::Globals::get()->m_scan_response_reader_proxy.scan(m_request);
}
void CScanButton::sl_sig_done(const std::shared_ptr<grpc::Status> status, std::shared_ptr<utils::CFile> file, std::shared_ptr<utils::ScannerParameters> params) {
    if (!status->ok()) {
        g_logger.log(WARN, "Scan failed");
    }
    setDisabled(false);
}

void CScanButton::sl_scanners_changed(const sane_in_the_membrane::utils::SharedAccessGuard<std::vector<std::shared_ptr<service::CScannerItem>>>& items) {
    setDisabled(items->empty());
}
