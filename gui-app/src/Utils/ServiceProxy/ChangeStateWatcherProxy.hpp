#ifndef UTILS_SERVICE_PROXY_CHANGE_STATE_WATCHER_PROXY
#define UTILS_SERVICE_PROXY_CHANGE_STATE_WATCHER_PROXY

#include "ServiceProxyBase.hpp"
#include "../../Service/ChannelStateChangeService.hpp"

namespace sane_in_the_membrane::utils::proxy {
    class CChangeStateWatcher : public CServiceProxyBase<service::CChangeStateWatcher> {
        Q_OBJECT

      public:
        CChangeStateWatcher(std::shared_ptr<service::CChangeStateWatcher>& service) : CServiceProxyBase(service) {
            set_connections();
        }

        virtual void set_connections() override {
            QObject::connect(m_service.get(), &service::CChangeStateWatcher::sig_channel_state_changed, this, &CChangeStateWatcher::sig_channel_state_changed);
            QObject::connect(m_service.get(), &service::CChangeStateWatcher::sig_stopping_auto_discovery, this, &CChangeStateWatcher::sig_stopping_auto_discovery);
        }

      signals:
        void sig_channel_state_changed(service::CChangeStateWatcher::CChannelState state);
        void sig_stopping_auto_discovery();
    };
}

#endif // !UTILS_SERVICE_PROXY_CHANGE_STATE_WATCHER_PROXY
