#pragma once
#ifndef SCAN_BUTTON_H
#define SCAN_BUTTON_H

#include <QPushButton>
#include <QWidget>
#include <QString>
#include <qtmetamacros.h>
#include "helloworld/v1/helloworld.grpc.pb.h"
#include <grpcpp/client_context.h>
#include "helloworld/v1/helloworld.pb.h"

using namespace helloworld::v1;

class CScanButton : public QPushButton {
  public:
    CScanButton(QWidget* parent, GreeterService::Stub& s);

  signals:
    void click_callback() {
        grpc::ClientContext context{};
        SayHelloResponse    response{};
        auto                status = stub.SayHello(&context, SayHelloRequest{}, &response);

        if (status.ok()) {
            std::cout << response.message() << "\n";
        }
    }

  private:
    GreeterService::Stub& stub;
};

#endif // !SCAN_BUTTON_H
