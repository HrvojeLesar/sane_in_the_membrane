#include "scan-ui/scan-button.hpp"
#include "scanner/v1/scanner.grpc.pb.h"
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QMainWindow>
#include <chrono>
#include <grpcpp/client_context.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/support/stub_options.h>
#include <iostream>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpushbutton.h>

using namespace scanner::v1;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QMainWindow  window;

    auto         channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    channel->WaitForConnected(std::chrono::system_clock::now() + std::chrono::seconds(5));
    if (channel.get() == nullptr) {
        std::cerr << "Failed to create channel" << std::endl;
        return 1;
    }

    ScannerService::Stub stub{channel};

    CScanButton          button(&window, stub);
    QObject::connect(&button, &CScanButton::clicked, &button, &CScanButton::click_callback);
    window.show();

    QApplication::exec();

    return 0;
}
