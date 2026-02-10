#ifndef SERVICE_REFRESH_SCANNERS_SERVICE
#define SERVICE_REFRESH_SCANNERS_SERVICE

#include "scanner/v1/scanner.grpc.pb.h"
#include <grpcpp/client_context.h>
#include <memory>
#include <QObject>

namespace sane_in_the_membrane::service {
    class CRefreshScannersService : public QObject {
        Q_OBJECT
      public:
        CRefreshScannersService(std::shared_ptr<scanner::v1::ScannerService::Stub>& stub);
        ~CRefreshScannersService();

        void refresh_scanners();

      signals:
        void sig_refresh_scanners_failed();
        void sig_refresh_scanners(std::shared_ptr<scanner::v1::RefreshScannersResponse>);

      private:
        std::shared_ptr<scanner::v1::ScannerService::Stub> m_stub;
    };
}

#endif // !SERVICE_REFRESH_SCANNERS_SERVICE
