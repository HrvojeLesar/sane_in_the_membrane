#ifndef SCANNER_SERVICE_HEADER
#define SCANNER_SERVICE_HEADER

#include "SaneDevice.hpp"
#include "scanner/v1/scanner.grpc.pb.h"
#include "scanner/v1/scanner.pb.h"
#include <chrono>
#include <grpcpp/grpcpp.h>
#include <memory>
#include <mutex>
#include <vector>

namespace service {
    using namespace scanner::v1;

    class CScannerServiceImpl : public ScannerService::CallbackService {
      public:
        grpc::ServerUnaryReactor*               GetScanners(grpc::CallbackServerContext* context, const GetScannersRequest* request, GetScannersResponse* response) override;
        grpc::ServerWriteReactor<ScanResponse>* Scan(grpc::CallbackServerContext* context, const ScanRequest* request) override;
        grpc::ServerUnaryReactor* RefreshScanners(grpc::CallbackServerContext* context, const RefreshScannersRequest* request, RefreshScannersResponse* response) override;

      private:
        bool                               should_refresh_devices() const;
        std::shared_ptr<sane::CSaneDevice> find_device(const std::string& name) const;
        void                               refresh_devices();

      private:
        std::vector<std::weak_ptr<sane::CSaneDevice>>      m_devices{};
        std::mutex                                         m_mutex{};

        std::chrono::time_point<std::chrono::system_clock> m_last_device_fetch{};
    };
}

#endif // !SCANNER_SERVICE_HEADER
