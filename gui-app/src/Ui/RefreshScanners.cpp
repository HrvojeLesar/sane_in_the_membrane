#include "RefreshScanners.hpp"

using namespace sane_in_the_membrane::ui;

#include "../Service/GetScannersService.hpp"
#include "../Service/RefreshScannersService.hpp"

CRefreshButton::CRefreshButton(QWidget* parent) : QPushButton("Refresh", parent) {
    QObject::connect(this, &CRefreshButton::clicked, this, &CRefreshButton::on_clicked_get_scanners);
    QObject::connect(service::g_refresh_scanner_service.get(), &service::CRefreshScannersService::sig_refresh_scanners, this, &CRefreshButton::sl_refresh_scanners);
    QObject::connect(service::g_refresh_scanner_service.get(), &service::CRefreshScannersService::sig_refresh_scanners_failed, this, &CRefreshButton::sl_refresh_scanners_failed);

    QObject::connect(service::g_get_scanner_service.get(), &service::CGetScannersService::sig_get_scanners, this, &CRefreshButton::sl_get_scanners);
    QObject::connect(service::g_get_scanner_service.get(), &service::CGetScannersService::sig_get_scanners_failed, this, &CRefreshButton::sl_get_scanners_failed);
}

void CRefreshButton::sl_refresh_scanners_failed() {
    std::cout << "Refresh failed - button \n";
}

void CRefreshButton::sl_refresh_scanners() {
    get_scanners();
}

void CRefreshButton::sl_get_scanners_failed() {
    std::cout << "Get scanners failed - button \n";
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
    service::g_get_scanner_service->get_scanners();
}

void CRefreshButton::refresh_scanners() {
    service::g_refresh_scanner_service->refresh_scanners();
}
void CRefreshButton::enable_button() {
    setText("Refresh");
    setDisabled(false);
}

void CRefreshButton::refreshing() {
    setText("Refreshing...");
    setDisabled(true);
}
