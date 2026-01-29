#ifndef UI_MAIN_APP
#define UI_MAIN_APP

#include "MainWindow.hpp"
#include "scanner/v1/scanner.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <grpcpp/support/channel_arguments.h>
#include <iostream>
#include <memory>
#include <qapplication.h>
#include <qmainwindow.h>
#include <qpushbutton.h>
#include <thread>
#include "../Service/GetScannersService.hpp"
#include "../Service/RefreshScannersService.hpp"
#include "../Service/DeviceList.hpp"
#include "../Readers/ScanResponseReader.hpp"

namespace ui {
    class CMainApp {
      public:
        CMainApp(QApplication& app) : m_app(app) {

            grpc::ChannelArguments args;
            args.SetMaxReceiveMessageSize(50 * 1024 * 1024);

            m_channel                          = grpc::CreateCustomChannel("localhost:50051", grpc::InsecureChannelCredentials(), args);
            m_stub                             = std::make_shared<scanner::v1::ScannerService::Stub>(m_channel);
            service::g_get_scanner_service     = std::make_unique<service::CGetScannersService>(*m_stub);
            service::g_refresh_scanner_service = std::make_unique<service::CRefreshScannersService>(*m_stub);
            service::g_device_list             = std::make_unique<service::CDeviceList>();
            reader::g_scan_response_reader     = std::make_unique<reader::CScanResponseReader>(*m_stub);

            m_channel_thread = std::make_unique<std::thread>(&CMainApp::connect_channel, this);
            m_main_window    = new CMainWindow();
        }

        ~CMainApp() {
            delete m_main_window;
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
        QApplication&                                      m_app;
        std::shared_ptr<grpc::Channel>                     m_channel{nullptr};
        std::unique_ptr<std::thread>                       m_channel_thread{nullptr};
        std::shared_ptr<scanner::v1::ScannerService::Stub> m_stub{nullptr};

        CMainWindow*                                       m_main_window{nullptr};
    };
}

#endif // !UI_MAIN_APP
