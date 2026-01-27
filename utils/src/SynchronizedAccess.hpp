#ifndef SANE_IN_THE_MEMBRANE_UTILS_SYNCHRONIZED_ACCESS
#define SANE_IN_THE_MEMBRANE_UTILS_SYNCHRONIZED_ACCESS

#include <mutex>
#include <shared_mutex>
#include <type_traits>

namespace sane_in_the_membrane::utils {

    template <typename T, typename LockType = std::unique_lock<std::shared_mutex>>
    class AccessGuard {
      public:
        AccessGuard(T& object, std::shared_mutex& mutex) : m_object(&object), m_lock(mutex) {}
        AccessGuard(AccessGuard<T, LockType>&& other) : m_object(other.m_object), m_lock(std::move(other.m_lock)) {}

        AccessGuard<T, LockType>& operator=(AccessGuard<T, LockType>&& other) {
            this->m_object = other.m_object;
            this->m_lock   = std::move(other.m_lock);

            return *this;
        }

        T* operator->() const {
            return this->m_object;
        }

        T& operator*() const {
            return *this->m_object;
        }

      private:
        T*       m_object;
        LockType m_lock;
    };

    template <typename T>
    using SharedAccessGuard = AccessGuard<const T, std::shared_lock<std::shared_mutex>>;

    template <typename T>
    class UniqueAccess {
      public:
        UniqueAccess() : m_object() {}
        UniqueAccess(const T& object) : m_object(object) {}
        UniqueAccess(const T&& object) : m_object(std::move(object)) {}

        UniqueAccess<T>& operator=(const T& object) {
            m_object = object;

            return *this;
        }

        UniqueAccess<T>& operator=(T&& object) {
            m_object = std::move(object);

            return *this;
        }

        AccessGuard<T> access() const {
            return AccessGuard<T>(m_object, m_mutex);
        }

        template <typename O = T, typename = std::enable_if_t<!std::is_const_v<O>>>
        SharedAccessGuard<const O> shared_access() const {
            return SharedAccessGuard<const O>(m_object, m_mutex);
        }

      private:
        mutable T                 m_object;
        mutable std::shared_mutex m_mutex;
    };
}

#endif // !SANE_IN_THE_MEMBRANE_UTILS_SYNCHRONIZED_ACCESS
