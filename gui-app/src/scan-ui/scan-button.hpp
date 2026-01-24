#ifndef SCAN_BUTTON_H
#define SCAN_BUTTON_H

#include "../Readers/ScanResponseReader.hpp"
#include <QPushButton>
#include <QString>
#include <QWidget>
#include <iostream>
#include <qobject.h>
#include <qpushbutton.h>
#include <qtmetamacros.h>

#include "../Service/GetScannersService.hpp"
#include "../Service/RefreshScannersService.hpp"
#include "../Service/DeviceList.hpp"

using namespace scanner::v1;

class CScanButton : public QPushButton {
  public:
    CScanButton(QWidget* parent) : QPushButton("Scan", parent) {
        QObject::connect(this, &CScanButton::clicked, this, &CScanButton::on_clicked_get_scanners);
        QObject::connect(service::g_get_scanner_service.get(), &service::CGetScannersService::sig_get_scanners_failed, this, &CScanButton::sl_get_scanners_failed);
        QObject::connect(service::g_get_scanner_service.get(), &service::CGetScannersService::sig_get_scanners, this, &CScanButton::sl_get_scanners);
    }

    ~CScanButton() {}

  private slots:
    void sl_get_scanners_failed() {
        std::cout << "Failed to get scanners\n";
        setDisabled(false);
    }

    void sl_get_scanners(std::shared_ptr<scanner::v1::GetScannersResponse> response) {
        std::cout << "Got scanners: " << response->scanners().size() << "\n";
        setDisabled(false);
    }

  private:
    void on_clicked_get_scanners() {
        std::cout << "clicky\n";
        setDisabled(true);

        // if (service::g_device_list->get_scanners().empty()) {
        //     refresh_scanners();
        // }
        get_scanners();
    }

    void get_scanners() {
        service::g_get_scanner_service->get_scanners();
    }

    void refresh_scanners() {
        service::g_refresh_scanner_service->refresh_scanners();
    }

  private:
};

#endif // !SCAN_BUTTON_H
