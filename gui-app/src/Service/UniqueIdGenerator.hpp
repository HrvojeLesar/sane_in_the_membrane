#ifndef SERVICE_UNIQUE_ID_GENERATOR
#define SERVICE_UNIQUE_ID_GENERATOR

#include <cstddef>
#include <cstdint>

namespace sane_in_the_membrane::service {
    class CUniqueIdGenerator {
      private:
        static constexpr int8_t TIME_BITS   = 44;
        static constexpr int8_t SEQ_BITS    = 12;
        static constexpr int8_t THREAD_BITS = 8;

        static constexpr size_t TIME_MASK   = (1ULL << TIME_BITS) - 1;
        static constexpr size_t SEQ_MASK    = (1ULL << SEQ_BITS) - 1;
        static constexpr size_t THREAD_MASK = (1ULL << THREAD_BITS) - 1;

        static constexpr size_t SEQ_SHIFT  = THREAD_BITS;
        static constexpr size_t TIME_SHIFT = THREAD_BITS + SEQ_BITS;

      public:
        CUniqueIdGenerator() = delete;

        static size_t id();

      private:
        static thread_local size_t g_sequence_number;
        static thread_local size_t g_thread_hash;
    };
}

#endif // !SERVICE_UNIQUE_ID_GENERATOR
