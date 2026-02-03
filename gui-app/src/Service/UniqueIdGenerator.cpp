#include "UniqueIdGenerator.hpp"
#include <cstddef>
#include <functional>
#include <thread>

using namespace sane_in_the_membrane::service;

thread_local size_t CUniqueIdGenerator::g_sequence_number = 0;
thread_local size_t CUniqueIdGenerator::g_thread_hash     = 0;

size_t              CUniqueIdGenerator::id() {
    if (g_thread_hash == 0) {
        g_thread_hash = std::hash<std::thread::id>{}(std::this_thread::get_id());
    }

    auto   sequence_number = g_sequence_number++;
    auto   time_now        = static_cast<size_t>(std::chrono::system_clock::now().time_since_epoch().count());

    size_t id = 0;

    id |= (time_now & TIME_MASK) << TIME_SHIFT;
    id |= (sequence_number & SEQ_MASK) << SEQ_SHIFT;
    id |= (g_thread_hash & THREAD_MASK);

    return id;
}
