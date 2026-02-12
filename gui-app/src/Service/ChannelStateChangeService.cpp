#include "ChannelStateChangeService.hpp"

sane_in_the_membrane::service::CChangeStateWatcher::CChannelState::CChannelState() : m_state(std::nullopt) {}
sane_in_the_membrane::service::CChangeStateWatcher::CChannelState::CChannelState(grpc_connectivity_state state) : m_state(state) {}
const std::optional<grpc_connectivity_state> sane_in_the_membrane::service::CChangeStateWatcher::CChannelState::get() const {
    return m_state;
}
constexpr const char* sane_in_the_membrane::service::CChangeStateWatcher::CChannelState::as_str() const {
    if (m_state.has_value()) {
        switch (m_state.value()) {
            case GRPC_CHANNEL_IDLE: return "IDLE";
            case GRPC_CHANNEL_CONNECTING: return "CONNECTING";
            case GRPC_CHANNEL_READY: return "READY";
            case GRPC_CHANNEL_TRANSIENT_FAILURE: return "TRANSIENT_FAILURE";
            case GRPC_CHANNEL_SHUTDOWN: return "SHUTDOWN";
            default: return UNKNOWN_STATE;
        }
    } else {
        return UNKNOWN_STATE;
    }
}
const std::string sane_in_the_membrane::service::CChangeStateWatcher::CChannelState::to_string() const {
    return std::string(as_str());
}
void sane_in_the_membrane::service::CChangeStateWatcher::CChannelState::set(grpc_connectivity_state new_state) {
    m_state = new_state;
}

sane_in_the_membrane::service::CChangeStateWatcher::CChangeStateWatcher(std::shared_ptr<grpc::Channel> channel) : m_channel(channel), m_thread(nullptr), m_state(), m_interval(5) {}
sane_in_the_membrane::service::CChangeStateWatcher::CChangeStateWatcher(std::shared_ptr<grpc::Channel> channel, std::chrono::seconds interval) :
    m_channel(channel), m_thread(nullptr), m_state(), m_interval(interval) {}

sane_in_the_membrane::service::CChangeStateWatcher::~CChangeStateWatcher() {
    if (m_thread.get() != nullptr && m_thread->joinable()) {
        m_thread->join();
    }
}

void sane_in_the_membrane::service::CChangeStateWatcher::start() {
    if (!m_started) {
        m_started = true;
        m_thread  = std::make_unique<std::thread>(&CChangeStateWatcher::start_impl, this);
    }
}

void sane_in_the_membrane::service::CChangeStateWatcher::stop() {
    {
        std::lock_guard lock{m_mutex};
        m_started = false;
    }
    m_cv.notify_all();
}

void sane_in_the_membrane::service::CChangeStateWatcher::start_impl() {
    while (m_started) {
        {
            std::unique_lock lock{m_mutex};

            if (m_cv.wait_for(lock, m_interval, [this] { return !m_started; }))
                return;
        }
        auto state_guard = m_state.access();
        state_guard->set(m_channel->GetState(false));

        emit sig_channel_state_changed(*state_guard);
    }
}
