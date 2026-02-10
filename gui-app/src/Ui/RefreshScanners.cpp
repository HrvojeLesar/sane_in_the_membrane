#include "RefreshScanners.hpp"
#include "../Utils/Globals.hpp"
#include "../GlobalLogger.cpp"

using namespace sane_in_the_membrane::ui;

CRefreshButton::CRefreshButton(QWidget* parent) : QPushButton("Refresh", parent) {
    QObject::connect(this, &CRefreshButton::clicked, this, &CRefreshButton::on_clicked_get_scanners);
    QObject::connect(&utils::Globals::get()->m_refresh_scanner_service_proxy, &utils::proxy::CRefreshScannersProxy::sig_refresh_scanners, this,
                     &CRefreshButton::sl_refresh_scanners);
    QObject::connect(&utils::Globals::get()->m_refresh_scanner_service_proxy, &utils::proxy::CRefreshScannersProxy::sig_refresh_scanners_failed, this,
                     &CRefreshButton::sl_refresh_scanners_failed);

    QObject::connect(&utils::Globals::get()->m_get_scanner_service_proxy, &utils::proxy::CGetScannerServiceProxy::sig_get_scanners, this, &CRefreshButton::sl_get_scanners);
    QObject::connect(&utils::Globals::get()->m_get_scanner_service_proxy, &utils::proxy::CGetScannerServiceProxy::sig_get_scanners_failed, this,
                     &CRefreshButton::sl_get_scanners_failed);
}

void CRefreshButton::sl_refresh_scanners_failed() {
    g_logger.log(DEBUG, "Refresh failed - button");
    enable_button();
}

void CRefreshButton::sl_refresh_scanners() {
    get_scanners();
}

void CRefreshButton::sl_get_scanners_failed() {
    g_logger.log(DEBUG, "Get scanners failed - button");
    enable_button();
}

void CRefreshButton::sl_get_scanners(std::shared_ptr<scanner::v1::GetScannersResponse> response) {
    enable_button();
}

void CRefreshButton::on_clicked_get_scanners() {
    refreshing();
    refresh_scanners();
}

void CRefreshButton::get_scanners() {
    utils::Globals::get()->m_get_scanner_service_proxy.get_scanners();
}

void CRefreshButton::refresh_scanners() {
    utils::Globals::get()->m_refresh_scanner_service_proxy.refresh_scanners();
}
void CRefreshButton::enable_button() {
    setText("Refresh");
    setDisabled(false);
}

void CRefreshButton::refreshing() {
    setText("Refreshing...");
    setDisabled(true);
}
