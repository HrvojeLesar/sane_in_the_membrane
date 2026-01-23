#ifndef SERVICE_REFRESH_SCANNERS_SERVICE
#define SERVICE_REFRESH_SCANNERS_SERVICE

#include "scanner/v1/scanner.grpc.pb.h"
#include <functional>
#include <grpcpp/client_context.h>
#include <iostream>
#include <memory>

namespace service {
    class CRefreshScannersService {
      public:
        CRefreshScannersService(scanner::v1::ScannerService::Stub& stub) : m_stub(stub) {}
        ~CRefreshScannersService() {
            std::cout << "Destroying refresh scanners service\n";
        }

        void refresh_scanners(std::function<void()> callback) {
            auto context  = std::make_shared<grpc::ClientContext>();
            auto request  = std::make_shared<scanner::v1::RefreshScannersRequest>();
            auto response = std::make_shared<scanner::v1::RefreshScannersResponse>();
            std::cout << "Trying to refresh scanners\n";
            m_stub.async()->RefreshScanners(context.get(), request.get(), response.get(), [callback, context, request, response](grpc::Status status) {
                if (!status.ok()) {
                    std::cout << "Failed to refresh scanners\n";
                } else {
                    std::cout << "Refreshed scanners\n";
                }

                callback();
            });
        }

      private:
        scanner::v1::ScannerService::Stub& m_stub;
    };
}

#endif // !SERVICE_REFRESH_SCANNERS_SERVICE
