#ifndef UTILS_SERVICE_PROXY_SERVICE_PROXY_BASE
#define UTILS_SERVICE_PROXY_SERVICE_PROXY_BASE

#include <memory>
#include <qobject.h>
#include <qtmetamacros.h>
namespace sane_in_the_membrane::utils::proxy {
    template <typename T>
    class CServiceProxyBase : public QObject {
      protected:
        CServiceProxyBase() = delete;

        CServiceProxyBase(CServiceProxyBase&& other) : m_service(std::move(other.m_service)) {}
        CServiceProxyBase(const std::shared_ptr<T>& service) : m_service(service) {}

        virtual ~CServiceProxyBase() = default;

      public:
        virtual void replace_service(std::shared_ptr<T>& service) {
            QObject::disconnect(m_service.get(), nullptr, nullptr, nullptr);

            m_service = service;

            set_connections();
        }

        virtual void set_connections() = 0;

      protected:
        std::shared_ptr<T> m_service;
    };
}

#endif // !UTILS_SERVICE_PROXY_SERVICE_PROXY_BASE
