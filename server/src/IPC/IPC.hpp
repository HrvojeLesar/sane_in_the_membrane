#ifndef IPC_IPC
#define IPC_IPC

#include "SynchronizedAccess.hpp"
#include <atomic>
#include <string>

namespace sane_in_the_membrane::startup {
    class CStartupOrchestrator;
}

namespace sane_in_the_membrane::ipc {
    class CIPC {

        friend class startup::CStartupOrchestrator;

        static constexpr int LISTEN_QUEUE_NUM = 10;
        static constexpr int READ_BUFFER_SIZE = 1024;

      public:
        void initialize();
        void shutdown();

      private:
        const std::string ipc_path() const;
        const std::string process_request(std::string& request) const;

      private:
        void set_port(int port);

      private:
        std::atomic<bool>        m_shutdown{false};
        utils::UniqueAccess<int> m_server_port{0};
    };
}

#endif // !IPC_IPC
