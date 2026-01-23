#ifndef UI_MAIN_APP
#define UI_MAIN_APP

#include "MainWindow.hpp"
#include "scanner/v1/scanner.grpc.pb.h"
#include <condition_variable>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <qapplication.h>
#include <qmainwindow.h>
#include <qpushbutton.h>
#include <thread>

namespace ui {
    class CMainApp {
      public:
        CMainApp(int argc, char* argv[]) :
            m_app(argc, argv), m_channel(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials())), m_stub(m_channel), m_main_window(m_stub) {
            m_channel_thread = std::make_unique<std::thread>(&CMainApp::connect_channel, this);
        }

        ~CMainApp() {
            if (m_channel_thread.get() != nullptr && m_channel_thread->joinable()) {
                m_channel_thread->join();
            }
        }

        void exec() {
            m_app.exec();
        }

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
        std::condition_variable           m_cv{};
        scanner::v1::ScannerService::Stub m_stub;

        CMainWindow                       m_main_window;
    };
}

#endif // !UI_MAIN_APP
