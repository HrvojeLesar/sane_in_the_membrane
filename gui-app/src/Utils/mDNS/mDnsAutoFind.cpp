#include "mDnsAutoFind.hpp"
#include "../../GlobalLogger.cpp"

using namespace sane_in_the_membrane::utils::mdns;

CMDnsAutoFinder::CMDnsAutoFinder() :
    m_latch(1), m_worker([this]() {
        m_latch.count_down();
        while (true) {
            {
                std::unique_lock lock{m_mutex};
                m_condition_variable.wait(lock);

                if (m_stop)
                    break;
            }

            discover_inner();
        }
    }) {};
CMDnsAutoFinder::~CMDnsAutoFinder() {
    {
        std::lock_guard lock(m_mutex);
        m_stop = true;
    }
    m_condition_variable.notify_all();

    m_worker.join();
};

CMDnsAutoFinder& CMDnsAutoFinder::get_instance() {
    static CMDnsAutoFinder instance;

    return instance;
}

void CMDnsAutoFinder::discover() {
    m_latch.wait();
    m_condition_variable.notify_one();
}

void CMDnsAutoFinder::discover_inner() {
    CMDns mdns{};

    g_logger.log(INFO, "Discovering mdns records");

    emit sig_discovering();

    auto discover_result = mdns.discover_services();
    if (!discover_result.has_value()) {
        g_logger.log(ERR, discover_result.error());
        emit sig_discover_failed(discover_result.error());
        return;
    }

    auto query_result = mdns.query_services();
    if (!query_result.has_value()) {
        g_logger.log(ERR, discover_result.error());
        emit sig_query_failed(discover_result.error());
        return;
    }

    emit sig_mdns_discovered(query_result.value());
}
