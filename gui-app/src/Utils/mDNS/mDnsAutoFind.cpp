#include "mDnsAutoFind.hpp"
#include "../../GlobalLogger.cpp"
#include "../Globals.hpp"
#include <grpcpp/support/channel_arguments.h>

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
        g_logger.log(DEBUG, "mDNS worker exiting");
    }) {
    QObject::connect(this, &CMDnsAutoFinder::sig_mdns_discovered, this, [this](const std::vector<SQueryResult>& discovered_connections) {
        g_logger.log(INFO, "Trying to update mdns records");
        if (!discovered_connections.empty()) {
            grpc::ChannelArguments args{};
            args.SetMaxReceiveMessageSize(50 * 1024 * 1024);

            utils::Globals::get_instance().change_channel(discovered_connections.at(0).as_address_with_port(), grpc::InsecureChannelCredentials(), args);
        } else {
            g_logger.log(INFO, "Mdns records empty");
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

    g_logger.log(INFO, "Discovering mdns records");

    emit sig_discovering();

    auto discover_result = mdns.discover_services();
    if (!discover_result.has_value()) {
        g_logger.log(ERR, discover_result.error());
        emit sig_discover_failed(discover_result.error());
        return;
    }

    INTERRUPT_GUARD();

    auto query_result = mdns.query_services();
    if (!query_result.has_value()) {
        g_logger.log(ERR, discover_result.error());
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
    g_logger.log(DEBUG, "Received interrupt");
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
