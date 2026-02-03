#ifndef SERVICE_GET_SCANNERS_SERVICE
#define SERVICE_GET_SCANNERS_SERVICE

#include "scanner/v1/scanner.grpc.pb.h"
#include "scanner/v1/scanner.pb.h"
#include <grpcpp/client_context.h>
#include <memory>
#include <QObject>
#include <qtmetamacros.h>

namespace sane_in_the_membrane::service {
    class CGetScannersService : public QObject {
        Q_OBJECT
      public:
        CGetScannersService(scanner::v1::ScannerService::Stub& stub);

        void get_scanners();

      signals:
        void sig_get_scanners_failed();
        void sig_get_scanners(std::shared_ptr<scanner::v1::GetScannersResponse>);

      private:
        scanner::v1::ScannerService::Stub& m_stub;
    };

    inline std::unique_ptr<CGetScannersService> g_get_scanner_service{nullptr};
}

#endif // !SERVICE_GET_SCANNERS_SERVICE
