#ifndef UTILS_SERVICE_PROXY_DEVICE_LIST_PROXY
#define UTILS_SERVICE_PROXY_DEVICE_LIST_PROXY

#include "ServiceProxyBase.hpp"
#include "../../Service/DeviceList.hpp"

namespace sane_in_the_membrane::utils::proxy {
    class CDeviceListProxy : public CServiceProxyBase<service::CDeviceList> {
        Q_OBJECT

      public:
        CDeviceListProxy(std::shared_ptr<service::CDeviceList>& service) : CServiceProxyBase(service) {}

        virtual void set_connections() override {
            QObject::connect(m_service.get(), &service::CDeviceList::sig_scanners_changed, this, &CDeviceListProxy::sig_scanners_changed);
        }

      signals:
        void sig_scanners_changed(const SharedAccessGuard<std::vector<std::shared_ptr<service::CScannerItem>>>& scanners);
    };
}

#endif // !UTILS_SERVICE_PROXY_DEVICE_LIST_PROXY
