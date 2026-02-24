#include "mDnsAutoFind.hpp"
#include "../Globals.hpp"
#include <GLogger.hpp>
#include "../../Utils/GRpcChannelArguments.hpp"

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <csignal>
#endif

using namespace sane_in_the_membrane::utils::mdns;

// TODO: See if sighandlers work on windows
#if defined(__linux__) || defined(__APPLE__)
void sig_handler(int signal_number) {}
#else
VOID CALLBACK sig_handler(int signal_number) {}
#endif

#define INTERRUPT_GUARD(...)                                                                                                                                                       \
    do {                                                                                                                                                                           \
        if (m_interrupted) {                                                                                                                                                       \
            m_interrupted = false;                                                                                                                                                 \
            return __VA_ARGS__;                                                                                                                                                    \
        }                                                                                                                                                                          \
    } while (0)

CMDnsAutoFinder::CMDnsAutoFinder() :
    m_worker([this]() {

#if defined(__linux__) || defined(__APPLE__)
        signal(SIGUSR1, sig_handler);
#endif
        discover_inner();
        while (true) {
            {
                std::unique_lock lock{m_mutex};
                m_condition_variable.wait(lock);

                if (m_stop)
                    break;
            }

            discover_inner();
        }
        log::debug("mDNS worker exiting");
    }) {
    QObject::connect(this, &CMDnsAutoFinder::sig_mdns_discovered, this, [this](const std::vector<SQueryResult>& discovered_connections) {
        log::info("Trying to update mdns records");
        if (!discovered_connections.empty()) {
            utils::Globals::get_instance().change_channel(discovered_connections.at(0).as_address_with_port(), grpc::InsecureChannelCredentials(), default_channel_args());
        } else {
            log::info("Mdns records empty");
        }
    });
};

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
    m_condition_variable.notify_one();
}

void CMDnsAutoFinder::discover_inner() {
    CMDns mdns{};

    log::info("Discovering mdns records");

    emit sig_discovering();

    auto discover_result = mdns.discover_services();
    if (!discover_result.has_value()) {
        log::error(discover_result.error());
        emit sig_discover_failed(discover_result.error());
        return;
    }

    INTERRUPT_GUARD();

    auto query_result = mdns.query_services();
    if (!query_result.has_value()) {
        log::error(discover_result.error());
        emit sig_query_failed(discover_result.error());
        return;
    }

    emit sig_mdns_discovered(query_result.value());

    INTERRUPT_GUARD();
}

#ifdef _WIN32
void CMDnsAutoFinder::interrupt() {}
#else
void CMDnsAutoFinder::interrupt() {
    std::lock_guard lock(m_mutex);
    log::debug("Received interrupt");
    if (!m_worker.joinable())
        return;

    m_interrupted = true;
#if defined(__linux__) || defined(__APPLE__)
    pthread_kill(m_worker.native_handle(), SIGUSR1);
#else
    QueueUserAPC(MyAPC, m_worker.native_handle(), 10);
#endif
}
#endif
