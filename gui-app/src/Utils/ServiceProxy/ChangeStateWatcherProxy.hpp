#ifndef UTILS_SERVICE_PROXY_CHANGE_STATE_WATCHER_PROXY
#define UTILS_SERVICE_PROXY_CHANGE_STATE_WATCHER_PROXY

#include "ServiceProxyBase.hpp"
#include "../../Service/ChannelStateChangeService.hpp"

namespace sane_in_the_membrane::utils::proxy {
    class CChangeStateWatcher : public CServiceProxyBase<service::CChangeStateWatcher> {
        Q_OBJECT

      public:
        CChangeStateWatcher(std::shared_ptr<service::CChangeStateWatcher>& service) : CServiceProxyBase(service) {}

        virtual void set_connections() override {
            QObject::connect(m_service.get(), &service::CChangeStateWatcher::sig_channel_state_changed, this, &CChangeStateWatcher::sig_channel_state_changed);
        }

      signals:
        void sig_channel_state_changed(service::CChangeStateWatcher::CChannelState state);
    };
}

#endif // !UTILS_SERVICE_PROXY_CHANGE_STATE_WATCHER_PROXY
