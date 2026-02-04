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

namespace sane_in_the_membrane::utils {
    struct Globals {
        Globals(grpc::ChannelArguments args) :
            m_channel(grpc::CreateCustomChannel("localhost:50051", grpc::InsecureChannelCredentials(), args)), m_stub(m_channel), m_get_scanner_service(m_stub),
            m_refresh_scanner_service(m_stub), m_device_list(m_get_scanner_service), m_scan_response_reader(m_stub), m_state_change_watcher(m_channel), m_file_manager() {}

        std::shared_ptr<grpc::Channel>    m_channel;
        scanner::v1::ScannerService::Stub m_stub;
        service::CGetScannersService      m_get_scanner_service;
        service::CRefreshScannersService  m_refresh_scanner_service;
        service::CDeviceList              m_device_list;
        reader::CScanResponseReader       m_scan_response_reader;
        service::CChangeStateWatcher      m_state_change_watcher;
        service::CFileManager             m_file_manager;

        static std::shared_ptr<Globals>   get() {
            if (g_instance == nullptr) {
                std::cerr << "Use of globals before initialization\n";
                std::terminate();
            }

            return g_instance;
        }

        static std::shared_ptr<Globals> init(grpc::ChannelArguments args) {
            if (g_instance.get() == nullptr) {
                g_instance = std::make_shared<utils::Globals>(args);
            }

            return g_instance;
        }

      private:
        static std::shared_ptr<Globals> g_instance;
    };
}

#endif // !UTILS_GLOBALS
