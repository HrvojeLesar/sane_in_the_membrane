#ifndef UI_MAIN_APP
#define UI_MAIN_APP

#include "MainWindow.hpp"
#include "scanner/v1/scanner.grpc.pb.h"
#include <grpcpp/channel.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/support/channel_arguments.h>
#include <memory>
#include <qapplication.h>
#include <qmainwindow.h>
#include <qpushbutton.h>
#include "../Utils/Globals.hpp"
#include "../Utils/mDNS/mDnsAutoFind.hpp"

namespace sane_in_the_membrane::ui {
    class CMainApp {

      public:
        CMainApp(QApplication& app) : m_app(app) {

            grpc::ChannelArguments args;
            args.SetMaxReceiveMessageSize(50 * 1024 * 1024);

            utils::Globals::get_instance().init("localhost:12345", nullptr, args);

            m_main_window = std::make_unique<CMainWindow>();
        }

        ~CMainApp() {
            utils::Globals::get_instance().channel_state_change_watcher()->stop();
        }

        int exec() {
            utils::Globals::get_instance().channel_state_change_watcher()->start();
            sane_in_the_membrane::utils::mdns::CMDnsAutoFinder::get_instance().discover();

            return m_app.exec();
        }

      private:
        QApplication&                                      m_app;
        std::shared_ptr<grpc::Channel>                     m_channel{nullptr};
        std::shared_ptr<scanner::v1::ScannerService::Stub> m_stub{nullptr};

        std::unique_ptr<CMainWindow>                       m_main_window{nullptr};
    };
}

#endif // !UI_MAIN_APP
