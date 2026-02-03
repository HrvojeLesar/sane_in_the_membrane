#ifndef UI_MAIN_APP
#define UI_MAIN_APP

#include "MainWindow.hpp"
#include "scanner/v1/scanner.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <grpcpp/support/channel_arguments.h>
#include <memory>
#include <qapplication.h>
#include <qmainwindow.h>
#include <qpushbutton.h>
#include "../Service/GetScannersService.hpp"
#include "../Service/RefreshScannersService.hpp"
#include "../Service/DeviceList.hpp"
#include "../Readers/ScanResponseReader.hpp"
#include "../Service/ChannelStateChangeService.hpp"

namespace sane_in_the_membrane::ui {
    class CMainApp {
      public:
        CMainApp(QApplication& app) : m_app(app) {

            grpc::ChannelArguments args;
            args.SetMaxReceiveMessageSize(50 * 1024 * 1024);

            m_channel                                             = grpc::CreateCustomChannel("localhost:50051", grpc::InsecureChannelCredentials(), args);
            m_stub                                                = std::make_shared<scanner::v1::ScannerService::Stub>(m_channel);
            service::g_get_scanner_service                        = std::make_unique<service::CGetScannersService>(*m_stub);
            service::g_refresh_scanner_service                    = std::make_unique<service::CRefreshScannersService>(*m_stub);
            service::g_device_list                                = std::make_unique<service::CDeviceList>();
            reader::g_scan_response_reader                        = std::make_unique<reader::CScanResponseReader>(*m_stub);
            sane_in_the_membrane::service::g_state_change_watcher = std::make_unique<sane_in_the_membrane::service::CChangeStateWatcher>(m_channel);
            sane_in_the_membrane::service::g_state_change_watcher->start();

            m_main_window = std::make_unique<CMainWindow>();
        }

        ~CMainApp() {
            sane_in_the_membrane::service::g_state_change_watcher->stop();
        }

        int exec() {
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
