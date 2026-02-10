#ifndef UTILS_SERVICE_PROXY_REFRESH_SCANNERS_PROXY
#define UTILS_SERVICE_PROXY_REFRESH_SCANNERS_PROXY

#include "ServiceProxyBase.hpp"
#include "../../Service/RefreshScannersService.hpp"

namespace sane_in_the_membrane::utils::proxy {
    class CRefreshScannersProxy : public CServiceProxyBase<service::CRefreshScannersService> {
        Q_OBJECT

      public:
        CRefreshScannersProxy(std::shared_ptr<service::CRefreshScannersService>& service) : CServiceProxyBase(service) {}

        virtual void set_connections() override {
            QObject::connect(m_service.get(), &service::CRefreshScannersService::sig_refresh_scanners, this, &CRefreshScannersProxy::sig_refresh_scanners);
            QObject::connect(m_service.get(), &service::CRefreshScannersService::sig_refresh_scanners_failed, this, &CRefreshScannersProxy::sig_refresh_scanners_failed);
        }

      signals:
        void sig_refresh_scanners_failed();
        void sig_refresh_scanners(std::shared_ptr<scanner::v1::RefreshScannersResponse>);
    };
}

#endif // !UTILS_SERVICE_PROXY_REFRESH_SCANNERS_PROXY
