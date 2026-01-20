#include "ThreadPool.hpp"

server::Threading::ThreadPool::ThreadPool(std::size_t thread_count) {
    m_threads.reserve(thread_count);
    while (thread_count > 0) {
        m_threads.emplace_back();
        --thread_count;
    }
}

void server::Threading::ThreadPool::shutdown() {
    m_cv.notify_all();

    for (auto& thread : m_threads) {
        thread.join();
    }
}
