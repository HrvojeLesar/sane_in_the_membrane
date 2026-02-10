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

#include "ServiceProxy/ChangeStateWatcherProxy.hpp"
#include "ServiceProxy/DeviceListProxy.hpp"
#include "ServiceProxy/GetScannerServiceProxy.hpp"
#include "ServiceProxy/RefreshScannersServiceProxy.hpp"
#include "ServiceProxy/ScanResponseReaderProxy.hpp"

namespace sane_in_the_membrane::utils {
    class CGlobalsChangeReactor;

    class Globals {
        struct CGlobalsInner;
        struct CProxies;

      public:
        Globals(CGlobalsInner inner) : m_inner(inner), m_proxies(new CProxies{inner}) {}
        Globals(CGlobalsInner inner, std::shared_ptr<CProxies> proxies) : m_inner(inner), m_proxies(std::move(proxies)) {}

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

        struct CProxies {
            CProxies() = delete;
            CProxies(CGlobalsInner& inner) :
                m_change_state_watcher_proxy(inner.m_services.m_state_change_watcher), m_device_list_proxy(inner.m_services.m_device_list),
                m_get_scanner_service_proxy(inner.m_services.m_get_scanner_service), m_refresh_scanner_service_proxy(inner.m_services.m_refresh_scanner_service),
                m_scan_response_reader_proxy(inner.m_services.m_scan_response_reader) {}

            proxy::CChangeStateWatcher      m_change_state_watcher_proxy;
            proxy::CDeviceListProxy         m_device_list_proxy;
            proxy::CGetScannerServiceProxy  m_get_scanner_service_proxy;
            proxy::CRefreshScannersProxy    m_refresh_scanner_service_proxy;
            proxy::CScanResponseReaderProxy m_scan_response_reader_proxy;
        };

        struct CGlobalsInner {
            CGlobalsInner(const std::string& connect_to, const std::shared_ptr<grpc::ChannelCredentials>& creds, const grpc::ChannelArguments args) :
                m_services(connect_to, creds, args) {}

            CServices m_services;
        };

      public:
        static std::shared_ptr<CProxies> get() {
            if (g_instance == nullptr) {
                SITM_ASSERT(0, "Use of globals before initialization");
            }

            return g_instance->m_proxies;
        }

        static std::shared_ptr<Globals> init(const std::string& connect_to, const std::shared_ptr<grpc::ChannelCredentials>& creds, const grpc::ChannelArguments args) {
            if (g_instance.get() == nullptr) {
                g_instance = std::make_shared<utils::Globals>(CGlobalsInner{connect_to, creds, args});
            }

            return g_instance;
        }

        static std::shared_ptr<Globals> change_channel(const std::string& connect_to, const std::shared_ptr<grpc::ChannelCredentials>& creds, const grpc::ChannelArguments args) {
            if (g_instance == nullptr) {
                SITM_ASSERT(0, "Use of globals before initialization");
            }

            g_instance->channel_state_change_watcher()->stop();
            g_instance = std::make_shared<utils::Globals>(CGlobalsInner{connect_to, creds, args}, std::move(g_instance->m_proxies));

            return g_instance;
        }

        static std::shared_ptr<service::CFileManager> file_manager() {
            if (g_instance == nullptr) {
                SITM_ASSERT(0, "Use of globals before initialization");
            }

            return g_instance->m_inner.m_services.m_file_manager;
        }

        static std::shared_ptr<service::CChangeStateWatcher> channel_state_change_watcher() {
            if (g_instance == nullptr) {
                SITM_ASSERT(0, "Use of globals before initialization");
            }

            return g_instance->m_inner.m_services.m_state_change_watcher;
        }

      private:
        static inline std::shared_ptr<Globals> g_instance{nullptr};
        CGlobalsInner                          m_inner;
        std::shared_ptr<CProxies>              m_proxies;
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
