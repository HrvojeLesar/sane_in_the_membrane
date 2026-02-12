#ifndef UTILS_GLOBALS
#define UTILS_GLOBALS

#include <atomic>
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

#include "ServiceProxy/ChangeStateWatcherProxy.hpp"
#include "ServiceProxy/DeviceListProxy.hpp"
#include "ServiceProxy/GetScannerServiceProxy.hpp"
#include "ServiceProxy/RefreshScannersServiceProxy.hpp"
#include "ServiceProxy/ScanResponseReaderProxy.hpp"

#include "../GlobalLogger.cpp"

namespace sane_in_the_membrane::utils {
    class CGlobalsChangeReactor;

    class Globals {
      public:
        Globals() : m_proxies(nullptr), m_services(nullptr) {}

      private:
        struct CServices {
            CServices(const std::string& connect_to, const std::shared_ptr<grpc::ChannelCredentials>& creds, const grpc::ChannelArguments args) :
                m_channel(grpc::CreateCustomChannel(connect_to, creds, args)), m_stub(std::make_shared<scanner::v1::ScannerService::Stub>(m_channel)),
                m_get_scanner_service(std::make_shared<service::CGetScannersService>(m_stub)),
                m_refresh_scanner_service(std::make_shared<service::CRefreshScannersService>(m_stub)), m_device_list(std::make_shared<service::CDeviceList>(m_get_scanner_service)),
                m_scan_response_reader(std::make_shared<reader::CScanResponseReader>(m_stub)), m_state_change_watcher(std::make_shared<service::CChangeStateWatcher>(m_channel)),
                m_file_manager() {}

            std::shared_ptr<grpc::Channel>                     m_channel;
            std::shared_ptr<scanner::v1::ScannerService::Stub> m_stub;
            std::shared_ptr<service::CGetScannersService>      m_get_scanner_service;
            std::shared_ptr<service::CRefreshScannersService>  m_refresh_scanner_service;
            std::shared_ptr<service::CDeviceList>              m_device_list;
            std::shared_ptr<reader::CScanResponseReader>       m_scan_response_reader;
            std::shared_ptr<service::CChangeStateWatcher>      m_state_change_watcher;
            std::shared_ptr<service::CFileManager>             m_file_manager;
        };

        struct SProxies {
            SProxies() = delete;
            SProxies(CServices& services) :
                m_change_state_watcher_proxy(services.m_state_change_watcher), m_device_list_proxy(services.m_device_list),
                m_get_scanner_service_proxy(services.m_get_scanner_service), m_refresh_scanner_service_proxy(services.m_refresh_scanner_service),
                m_scan_response_reader_proxy(services.m_scan_response_reader) {}

            void replace_services(CServices& services) {
                m_change_state_watcher_proxy.replace_service(services.m_state_change_watcher);
                m_device_list_proxy.replace_service(services.m_device_list);
                m_get_scanner_service_proxy.replace_service(services.m_get_scanner_service);
                m_refresh_scanner_service_proxy.replace_service(services.m_refresh_scanner_service);
                m_scan_response_reader_proxy.replace_service(services.m_scan_response_reader);
            }

            proxy::CChangeStateWatcher      m_change_state_watcher_proxy;
            proxy::CDeviceListProxy         m_device_list_proxy;
            proxy::CGetScannerServiceProxy  m_get_scanner_service_proxy;
            proxy::CRefreshScannersProxy    m_refresh_scanner_service_proxy;
            proxy::CScanResponseReaderProxy m_scan_response_reader_proxy;
        };

      public:
        static Globals& get_instance() {
            static Globals instance;

            return instance;
        }

        void init(const std::string& connect_to, const std::shared_ptr<grpc::ChannelCredentials>& creds, const grpc::ChannelArguments args) {
            if (m_initialized)
                return;

            m_services = std::make_unique<CServices>(connect_to, creds, args);
            m_proxies  = std::make_unique<SProxies>(*m_services);

            if (!m_initialized)
                m_initialized = true;
        }

        void change_channel(const std::string& connect_to, const std::shared_ptr<grpc::ChannelCredentials>& creds, const grpc::ChannelArguments args) {
            if (!m_initialized)
                SITM_ASSERT(0, "Use of globals before initialization");

            g_logger.log(INFO, std::format("Changing address to: {}", connect_to));

            m_services->m_state_change_watcher->stop();
            auto services = std::make_unique<CServices>(connect_to, creds, args);
            m_services.swap(services);
            m_services->m_state_change_watcher->start();

            m_proxies->replace_services(*m_services);
        }

        std::shared_ptr<service::CFileManager> file_manager() {
            if (!m_initialized)
                SITM_ASSERT(0, "Use of globals before initialization");

            return m_services->m_file_manager;
        }

        std::shared_ptr<service::CChangeStateWatcher> channel_state_change_watcher() {
            if (!m_initialized)
                SITM_ASSERT(0, "Use of globals before initialization");

            return m_services->m_state_change_watcher;
        }

        SProxies* proxies() const {
            return m_proxies.get();
        }

      private:
        std::unique_ptr<SProxies>  m_proxies;
        std::unique_ptr<CServices> m_services;
        std::atomic<bool>          m_initialized;
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
