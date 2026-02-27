#include "RefreshScanners.hpp"
#include "../../Utils/Globals.hpp"
#include <GLogger.hpp>
#include <qapplication.h>
#include "../../Utils/ErrorDialogue.hpp"

using namespace sane_in_the_membrane::ui::select;

CRefreshButton::CRefreshButton(QWidget* parent) : QPushButton("Refresh", parent) {
    QObject::connect(this, &CRefreshButton::clicked, this, &CRefreshButton::on_clicked_get_scanners);
    QObject::connect(&utils::Globals::get_instance().proxies()->m_refresh_scanner_service_proxy, &utils::proxy::CRefreshScannersProxy::sig_refresh_scanners, this,
                     &CRefreshButton::sl_refresh_scanners);
    QObject::connect(&utils::Globals::get_instance().proxies()->m_refresh_scanner_service_proxy, &utils::proxy::CRefreshScannersProxy::sig_refresh_scanners_failed, this,
                     &CRefreshButton::sl_refresh_scanners_failed);

    QObject::connect(&utils::Globals::get_instance().proxies()->m_get_scanner_service_proxy, &utils::proxy::CGetScannerServiceProxy::sig_get_scanners, this,
                     &CRefreshButton::sl_get_scanners);
    QObject::connect(&utils::Globals::get_instance().proxies()->m_get_scanner_service_proxy, &utils::proxy::CGetScannerServiceProxy::sig_get_scanners_failed, this,
                     &CRefreshButton::sl_get_scanners_failed);
}

void CRefreshButton::sl_refresh_scanners_failed() {
    log::warn("Refresh failed - button");
    utils::CErrorDialogue::show_message("Refresh failed");
    enable_button();
}

void CRefreshButton::sl_refresh_scanners() {
    get_scanners();
}

void CRefreshButton::sl_get_scanners_failed() {
    log::warn("Get scanners failed - button");
    utils::CErrorDialogue::show_message("Failed to get scanners");
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
    utils::Globals::get_instance().proxies()->m_get_scanner_service_proxy.get_scanners();
}

void CRefreshButton::refresh_scanners() {
    utils::Globals::get_instance().proxies()->m_refresh_scanner_service_proxy.refresh_scanners();
}
void CRefreshButton::enable_button() {
    setText("Refresh");
    setDisabled(false);
}

void CRefreshButton::refreshing() {
    setText("Refreshing...");
    setDisabled(true);
}
