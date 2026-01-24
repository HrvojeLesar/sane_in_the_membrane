#ifndef SERVICE_GET_SCANNERS_SERVICE
#define SERVICE_GET_SCANNERS_SERVICE

#include "scanner/v1/scanner.grpc.pb.h"
#include "scanner/v1/scanner.pb.h"
#include <grpcpp/client_context.h>
#include <memory>
#include <QObject>
#include <qtmetamacros.h>

namespace service {
    class CGetScannersService : public QObject {
        Q_OBJECT
      public:
        CGetScannersService(scanner::v1::ScannerService::Stub& stub) : m_stub(stub) {}

        void get_scanners() {
            auto context  = std::make_shared<grpc::ClientContext>();
            auto request  = std::make_shared<scanner::v1::GetScannersRequest>();
            auto response = std::make_shared<scanner::v1::GetScannersResponse>();
            m_stub.async()->GetScanners(context.get(), request.get(), response.get(), [context, request, response, this](grpc::Status status) {
                if (!status.ok()) {
                    emit this->sig_get_scanners_failed();
                } else {
                    emit this->sig_get_scanners(std::move(response));
                }
            });
        }

      signals:
        void sig_get_scanners_failed();
        void sig_get_scanners(std::shared_ptr<scanner::v1::GetScannersResponse>);

      private:
        scanner::v1::ScannerService::Stub& m_stub;
    };

    inline std::unique_ptr<CGetScannersService> g_get_scanner_service{nullptr};
}

#endif // !SERVICE_GET_SCANNERS_SERVICE
