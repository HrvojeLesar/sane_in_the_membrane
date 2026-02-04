#ifndef SERVICE_CHANNEL_STATE_CHANGE_SERVICE
#define SERVICE_CHANNEL_STATE_CHANGE_SERVICE

#include <atomic>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/completion_queue.h>
#include <memory>
#include <qobject.h>
#include <thread>
#include "SynchronizedAccess.hpp"

namespace sane_in_the_membrane::service {
    class CChangeStateWatcher : public QObject {
      private:
        class CChannelState {
          private:
            static constexpr const char* UNKNOWN_STATE = "UNKNOWN";

          public:
            CChannelState();
            CChannelState(grpc_connectivity_state state);

            const std::optional<grpc_connectivity_state> get() const;
            constexpr const char*                        as_str() const;
            const std::string                            to_string() const;
            void                                         set(grpc_connectivity_state new_state);

          private:
            std::optional<grpc_connectivity_state> m_state;
        };

        Q_OBJECT

        static constexpr std::chrono::duration<int64_t> TIMEOUT_TIME = std::chrono::seconds(5);

      public:
        CChangeStateWatcher(std::shared_ptr<grpc::Channel> channel);
        ~CChangeStateWatcher();

        void start();
        void stop();

      signals:
        void sig_channel_state_changed(CChannelState state);

      private:
        void                               start_impl();

        std::shared_ptr<grpc::Channel>     m_channel;
        grpc::CompletionQueue              m_completion_queue{};
        std::unique_ptr<std::thread>       m_thread;
        std::atomic<bool>                  m_started{false};
        utils::UniqueAccess<CChannelState> m_state{};
    };
}

#endif // !SERVICE_CHANNEL_STATE_CHANGE_SERVICE
