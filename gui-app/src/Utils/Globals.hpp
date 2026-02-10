#ifndef UTILS_GLOBALS
#define UTILS_GLOBALS

#include <grpcpp/channel.h>
#include <grpcpp/grpcpp.h>
#include <memory>
#include "../Service/GetScannersService.hpp"
#include "../Service/RefreshScannersService.hpp"
#include "../Service/DeviceList.hpp"
#include "../Readers/ScanResponseReader.hpp"
#include "../Service/ChannelStateChangeService.hpp"
#include "../Service/FileManager.hpp"
#include <Assert.hpp>

namespace sane_in_the_membrane::utils {
    class CGlobalsChangeReactor;

    class Globals {
      public:
        Globals(const std::string& connect_to, const std::shared_ptr<grpc::ChannelCredentials>& creds, const grpc::ChannelArguments args) :
            m_channel(grpc::CreateCustomChannel(connect_to, creds, args)), m_stub(std::make_shared<scanner::v1::ScannerService::Stub>(m_channel)),
            m_get_scanner_service(std::make_shared<service::CGetScannersService>(m_stub)), m_refresh_scanner_service(std::make_shared<service::CRefreshScannersService>(m_stub)),
            m_device_list(std::make_shared<service::CDeviceList>(m_get_scanner_service)), m_scan_response_reader(std::make_shared<reader::CScanResponseReader>(m_stub)),
            m_state_change_watcher(std::make_shared<service::CChangeStateWatcher>(m_channel)), m_file_manager() {}

      private:
        std::shared_ptr<grpc::Channel>                     m_channel;
        std::shared_ptr<scanner::v1::ScannerService::Stub> m_stub;
        std::shared_ptr<service::CGetScannersService>      m_get_scanner_service;
        std::shared_ptr<service::CRefreshScannersService>  m_refresh_scanner_service;
        std::shared_ptr<service::CDeviceList>              m_device_list;
        std::shared_ptr<reader::CScanResponseReader>       m_scan_response_reader;
        std::shared_ptr<service::CChangeStateWatcher>      m_state_change_watcher;
        std::shared_ptr<service::CFileManager>             m_file_manager;

      public:
        static std::shared_ptr<Globals> get() {
            if (g_instance == nullptr) {
                SITM_ASSERT(0, "Use of globals before initialization");
            }

            return g_instance;
        }

        static std::shared_ptr<Globals> init(const std::string& connect_to, const std::shared_ptr<grpc::ChannelCredentials>& creds, const grpc::ChannelArguments args) {
            if (g_instance.get() == nullptr) {
                g_instance = std::make_shared<utils::Globals>(connect_to, creds, args);
            }

            return g_instance;
        }

        static std::shared_ptr<Globals> change_channel(const std::string& connect_to, const std::shared_ptr<grpc::ChannelCredentials>& creds, const grpc::ChannelArguments args) {
            if (g_instance == nullptr) {
                SITM_ASSERT(0, "Use of globals before initialization");
            }

            g_instance = std::make_shared<utils::Globals>(connect_to, creds, args);

            return g_instance;
        }

      private:
        static inline std::shared_ptr<Globals> g_instance{nullptr};
    };

    class CGlobalsChangeReactor : public QObject {
        Q_OBJECT

      public:
        void emit_globals(std::shared_ptr<Globals>& globals) {
            emit sig_globals_changed(globals);
        }

      signals:
        void sig_globals_changed(std::shared_ptr<Globals>& globals);
    };

    static inline std::unique_ptr<CGlobalsChangeReactor> g_globals_change_reactor{};
}

#endif // !UTILS_GLOBALS
