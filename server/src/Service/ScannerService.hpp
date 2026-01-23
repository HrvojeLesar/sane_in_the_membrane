#ifndef SCANNER_SERVICE_HEADER
#define SCANNER_SERVICE_HEADER

#include "SaneDevice.hpp"
#include "scanner/v1/scanner.grpc.pb.h"
#include "scanner/v1/scanner.pb.h"
#include <chrono>
#include <grpcpp/server_context.h>
#include <grpcpp/support/server_callback.h>
#include <memory>
#include <mutex>
#include <vector>

namespace service {
    using namespace scanner::v1;

    class CScannerServiceImpl : public ScannerService::CallbackService {
      private:
        grpc::ServerUnaryReactor* GetScanners(grpc::CallbackServerContext* context, const GetScannersRequest* request, GetScannersResponse* response) override;

      private:
        bool should_refetch_devices() const;

      private:
        std::vector<std::weak_ptr<sane::CSaneDevice>>      m_devices{};
        std::mutex                                         m_mutex{};

        std::chrono::time_point<std::chrono::system_clock> m_last_device_fetch{};
    };
}

#endif // !SCANNER_SERVICE_HEADER
