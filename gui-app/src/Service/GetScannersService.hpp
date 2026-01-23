#ifndef SERVICE_GET_SCANNERS_SERVICE
#define SERVICE_GET_SCANNERS_SERVICE

#include "scanner/v1/scanner.grpc.pb.h"
#include <grpcpp/client_context.h>
#include <memory>

namespace service {
    class CGetScannersService {
      public:
        CGetScannersService(scanner::v1::ScannerService::Stub& stub) : m_stub(stub) {}

        void refresh_scanners(std::function<void()> callback) {
            auto context  = std::make_shared<grpc::ClientContext>();
            auto request  = std::make_shared<scanner::v1::GetScannersRequest>();
            auto response = std::make_shared<scanner::v1::GetScannersResponse>();
            m_stub.async()->GetScanners(context.get(), request.get(), response.get(), [context, request, response, callback](grpc::Status status) {
                if (!status.ok()) {
                    std::cout << "Failed to get scanners\n";
                }

                auto scanners = response->scanners();

                for (const auto& scanner : scanners) {
                    std::cout << scanner.name() << "\n";
                }

                callback();
            });
        }

      private:
        scanner::v1::ScannerService::Stub& m_stub;
    };
}

#endif // !SERVICE_GET_SCANNERS_SERVICE
