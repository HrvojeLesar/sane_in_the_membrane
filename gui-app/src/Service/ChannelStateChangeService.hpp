#ifndef SERVICE_CHANNEL_STATE_CHANGE_SERVICE
#define SERVICE_CHANNEL_STATE_CHANGE_SERVICE

#include <atomic>
#include <condition_variable>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/completion_queue.h>
#include <memory>
#include <mutex>
#include <qobject.h>
#include <thread>
#include "SynchronizedAccess.hpp"

namespace sane_in_the_membrane::utils::proxy {
    class CChangeStateWatcher;
}

namespace sane_in_the_membrane::service {
    class CChangeStateWatcher : public QObject {
      public:
        class CChannelState {
          private:
            static constexpr const char* UNKNOWN_STATE = "UNKNOWN";

          public:
            CChannelState();
            CChannelState(grpc_connectivity_state state);

            const grpc_connectivity_state get() const;
            constexpr const char*         as_str() const;
            const std::string             to_string() const;
            void                          set(grpc_connectivity_state new_state);

          private:
            grpc_connectivity_state m_state;
        };

      private:
        Q_OBJECT

        static constexpr std::chrono::duration<int64_t> TIMEOUT_TIME           = std::chrono::seconds(5);
        static constexpr size_t                         AUTO_CONNECT_MAX_TRIES = 5;

      public:
        CChangeStateWatcher(std::shared_ptr<grpc::Channel> channel);
        CChangeStateWatcher(std::shared_ptr<grpc::Channel> channel, std::chrono::seconds interval);
        ~CChangeStateWatcher();

        void start();
        void stop();

      signals:
        void sig_channel_state_changed(CChannelState state);
        void sig_stopping_auto_discovery();

      private:
        void                               start_impl();

        std::shared_ptr<grpc::Channel>     m_channel;
        std::unique_ptr<std::thread>       m_thread;
        std::atomic<bool>                  m_started{false};
        utils::UniqueAccess<CChannelState> m_state{};
        std::chrono::seconds               m_interval{};
        std::mutex                         m_mutex{};
        std::condition_variable            m_cv{};
        utils::UniqueAccess<size_t>        m_connection_failure_count{0};
    };
}

#endif // !SERVICE_CHANNEL_STATE_CHANGE_SERVICE
