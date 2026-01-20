#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <thread>
#include <vector>

namespace server::Threading {
    class ThreadPool final {
      public:
        ThreadPool(std::size_t thread_count);
        void shutdown();
        ~ThreadPool();

      private:
        std::vector<std::thread> m_threads{};
        std::mutex               m_mutex{};
        std::condition_variable  m_cv{};
    };
}
