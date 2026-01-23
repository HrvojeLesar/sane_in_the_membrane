#ifndef SCAN_BUTTON_H
#define SCAN_BUTTON_H

#include "../Readers/ScanResponseReader.hpp"
#include "scanner/v1/scanner.grpc.pb.h"
#include <QPushButton>
#include <QString>
#include <QWidget>
#include <grpcpp/client_context.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qtmetamacros.h>

#include "../Service/RefreshScannersService.hpp"
#include "../Service/GetScannersService.hpp"

using namespace scanner::v1;

class CScanButton : public QPushButton {
  public:
    CScanButton(QWidget* parent, ScannerService::Stub& stub) : QPushButton("Scan", parent), m_stub(stub), m_refresh_scanners_service(m_stub), m_get_scanners_service(m_stub) {
        m_other_button = new QPushButton("cu", parent);
        m_other_button->setGeometry(20, 20, 100, 30);
        QObject::connect(this, &CScanButton::clicked, this, &CScanButton::click_callback);
        QObject::connect(m_other_button, &QPushButton::clicked, this, &CScanButton::get_scanners);
    }

    ~CScanButton() {
        delete m_other_button;
    }

  signals:
    void click_callback() {
        setDisabled(true);
        m_refresh_scanners_service.refresh_scanners([this]() {
            std::cout << "Calling callback\n";
            this->setDisabled(false);
        });
    }

    void get_scanners() {
        m_other_button->setDisabled(true);
        m_get_scanners_service.refresh_scanners([this]() {
            std::cout << "Calling callback\n";
            this->m_other_button->setDisabled(false);
        });
    }

  private:
    ScannerService::Stub&            m_stub;
    service::CRefreshScannersService m_refresh_scanners_service;
    service::CGetScannersService     m_get_scanners_service;
    QPushButton*                     m_other_button;
};

#endif // !SCAN_BUTTON_H
