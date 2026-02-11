#ifndef UTILS_MDNS_MDNS_AUTOFIND
#define UTILS_MDNS_MDNS_AUTOFIND

#include <atomic>
#include <condition_variable>
#include <latch>
#include <qobject.h>
#include "mDns.hpp"

namespace sane_in_the_membrane::utils::mdns {
    class CMDnsAutoFinder : public QObject {
        Q_OBJECT

      private:
        CMDnsAutoFinder();
        ~CMDnsAutoFinder();
        CMDnsAutoFinder(CMDnsAutoFinder&)  = delete;
        CMDnsAutoFinder(CMDnsAutoFinder&&) = delete;
        void operator=(CMDnsAutoFinder&)   = delete;
        void operator=(CMDnsAutoFinder&&)  = delete;

        void discover_inner();

      public:
        static CMDnsAutoFinder& get_instance();
        void                    discover();

      signals:
        void sig_discovering();
        void sig_mdns_discovered(const std::vector<SQueryResult> discovered_connections);
        void sig_discover_failed(const std::string& error);
        void sig_query_failed(const std::string& error);

      private:
        std::atomic<bool>       m_stop{false};
        std::condition_variable m_condition_variable{};
        std::mutex              m_mutex{};
        std::latch              m_latch;
        std::thread             m_worker;
    };
}

#endif
