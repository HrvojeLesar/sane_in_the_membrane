#ifndef UI_MAIN_APP
#define UI_MAIN_APP

#include "MainWindow.hpp"
#include "scanner/v1/scanner.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <qapplication.h>
#include <qmainwindow.h>
#include <qpushbutton.h>
#include <thread>
#include "../Service/GetScannersService.hpp"
#include "../Service/RefreshScannersService.hpp"

namespace ui {
    class CMainApp {
      public:
        CMainApp(int argc, char* argv[]) : m_app(argc, argv), m_channel(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials())), m_stub(m_channel) {
            service::g_get_scanner_service     = std::make_unique<service::CGetScannersService>(m_stub);
            service::g_refresh_scanner_service = std::make_unique<service::CRefreshScannersService>(m_stub);

            m_channel_thread = std::make_unique<std::thread>(&CMainApp::connect_channel, this);
            m_main_window    = std::make_unique<CMainWindow>();
        }

        ~CMainApp() {
            if (m_channel_thread.get() != nullptr && m_channel_thread->joinable()) {
                m_channel_thread->join();
            }
        }

        int exec() {
            return m_app.exec();
        }

      private:
        void connect_channel() {
            if (!m_channel->WaitForConnected(std::chrono::system_clock::now() + std::chrono::seconds(5))) {
                std::cout << "Could not connect to server\n";
                // TODO: Display error to user
            } else {
                std::cout << "Connected to server\n";
            }
        }

      private:
        QApplication                      m_app;
        std::shared_ptr<grpc::Channel>    m_channel{};
        std::unique_ptr<std::thread>      m_channel_thread{};
        scanner::v1::ScannerService::Stub m_stub;

        std::unique_ptr<CMainWindow>      m_main_window;
    };
}

#endif // !UI_MAIN_APP
