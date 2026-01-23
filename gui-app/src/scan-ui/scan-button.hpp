#pragma once
#ifndef SCAN_BUTTON_H
#define SCAN_BUTTON_H

#include <QPushButton>
#include <QWidget>
#include <QString>
#include <qtmetamacros.h>
#include <grpcpp/client_context.h>
#include "scanner/v1/scanner.pb.h"
#include "scanner/v1/scanner.grpc.pb.h"

using namespace scanner::v1;

class CScanButton : public QPushButton {
  public:
    CScanButton(QWidget* parent, ScannerService::Stub& s);

  signals:
    void click_callback() {
        auto context  = new grpc::ClientContext{};
        auto response = new GetScannersResponse{};
        auto request  = new GetScannersRequest{};

        std::cout << "Getting scanners:\n";

        m_stub.async()->GetScanners(context, request, response, [request, response, context](grpc::Status status) {
            if (!status.ok()) {
                std::cout << "No scan\n";
                return;
            }

            std::cout << "Was\n";
            auto scanners = response->scanners();

            std::cout << "Wijst\n";
            for (const auto& scanner : scanners) {
                std::cout << scanner.name() << "\n";
            }

            delete request;
            delete response;
            delete context;
        });
        std::cout << "Exited function\n";
    }

  private:
    ScannerService::Stub& m_stub;
};

#endif // !SCAN_BUTTON_H
