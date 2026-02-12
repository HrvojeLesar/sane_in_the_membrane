#include "mDnsAutoFind.hpp"
#include "../../GlobalLogger.cpp"
#include "../Globals.hpp"
#include <grpcpp/support/channel_arguments.h>

using namespace sane_in_the_membrane::utils::mdns;

CMDnsAutoFinder::CMDnsAutoFinder() :
    m_worker([this]() {
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

    auto query_result = mdns.query_services();
    if (!query_result.has_value()) {
        g_logger.log(ERR, discover_result.error());
        emit sig_query_failed(discover_result.error());
        return;
    }

    emit sig_mdns_discovered(query_result.value());
}
