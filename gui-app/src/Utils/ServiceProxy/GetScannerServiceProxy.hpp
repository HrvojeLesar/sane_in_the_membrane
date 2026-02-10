#ifndef UTILS_SERVICE_PROXY_GET_SCANNER_SERVICE_PROXY
#define UTILS_SERVICE_PROXY_GET_SCANNER_SERVICE_PROXY

#include "ServiceProxyBase.hpp"
#include "../../Service/GetScannersService.hpp"

namespace sane_in_the_membrane::utils::proxy {
    class CRefreshScannersProxy : public CServiceProxyBase<service::CGetScannersService> {
        Q_OBJECT

      public:
        CRefreshScannersProxy(std::shared_ptr<service::CGetScannersService>& service) : CServiceProxyBase(service) {}

        virtual void set_connections() override {
            QObject::connect(m_service.get(), &service::CGetScannersService::sig_get_scanners, this, &CRefreshScannersProxy::sig_get_scanners);
            QObject::connect(m_service.get(), &service::CGetScannersService::sig_get_scanners_failed, this, &CRefreshScannersProxy::sig_get_scanners_failed);
        }

      signals:
        void sig_get_scanners_failed();
        void sig_get_scanners(std::shared_ptr<scanner::v1::GetScannersResponse>);
    };
}

#endif // !UTILS_SERVICE_PROXY_GET_SCANNER_SERVICE_PROXY
