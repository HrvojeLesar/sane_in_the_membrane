#ifndef MDNS_AVAHI_SERVICE_REGISTRATION
#define MDNS_AVAHI_SERVICE_REGISTRATION

#include <atomic>
#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-common/address.h>
#include <avahi-common/defs.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/error.h>
#include <cassert>
#include <cstddef>
#include <expected>
#include <format>
#include "../GlobalLogger.cpp"
#include <SynchronizedAccess.hpp>
#include <optional>

namespace sane_in_the_membrane::mdns {

    class CAvahiService {
        static constexpr char BASE_SERVICE_NAME[]{"sane_in_the_membrane_service"};
        static constexpr char SERVICE_TYPE[]{"_sane_in_the_membrane_service._tcp"};
        static constexpr int  AVAHI_NO_FLAGS{0};

        static void           s_client_callback(AvahiClient* client, AvahiClientState state, void* userdata) {
            assert(client);

            auto service = static_cast<CAvahiService*>(userdata);
            service->client_callback(client, state);
        }

        static void s_group_callback(AvahiEntryGroup* group, AvahiEntryGroupState state, void* userdata) {
            assert(group);

            auto service = static_cast<CAvahiService*>(userdata);
            service->group_callback(group, state);
        }

      public:
        CAvahiService(int port = 50051) : m_simple_poll(avahi_simple_poll_new()), m_port(port) {
            g_logger->log(DEBUG, "Create avahi");
        }

        ~CAvahiService() {
            g_logger->log(DEBUG, "Destroy avahi");
            if (m_client)
                avahi_client_free(m_client);
            if (m_simple_poll)
                avahi_simple_poll_free(m_simple_poll);
        }

        std::expected<void, int> start_and_wait() {
            g_logger->log(DEBUG, "Starting Avahi service registration");
            int error{};
            m_client = avahi_client_new(avahi_simple_poll_get(m_simple_poll), AvahiClientFlags::AVAHI_CLIENT_NO_FAIL, s_client_callback, this, &error);

            if (error < 0) {
                g_logger->log(ERR, std::format("Avahi service registration failed with errorno: {}", error));
                return std::unexpected(error);
            }

            avahi_simple_poll_loop(m_simple_poll);

            return {};
        }

        void shutdown() {
            if (m_simple_poll)
                avahi_simple_poll_quit(m_simple_poll);
        }

      private:
        void create_service() {
            if (!m_group)
                m_group = avahi_entry_group_new(m_client, s_group_callback, this);

            if (avahi_entry_group_is_empty(m_group)) {
                g_logger->log(INFO, std::format("Adding service '{}'", m_service_name));

                auto ret = avahi_entry_group_add_service(m_group, AVAHI_IF_UNSPEC, AVAHI_PROTO_INET, static_cast<AvahiPublishFlags>(0), m_service_name.c_str(), SERVICE_TYPE, NULL,
                                                         NULL, m_port, NULL);

                if (ret == AVAHI_ERR_COLLISION) {
                    g_logger->log(INFO, std::format("Avahi service collision '{}', changing name", m_service_name));
                    avahi_entry_group_reset(m_group);
                    create_service();

                    return;
                }

                if (ret != AVAHI_OK) {
                    g_logger->log(ERR, std::format("Failed to add {} service to avahi: {}", m_service_name, avahi_strerror(ret)));
                    return shutdown();
                }

                ret = avahi_entry_group_commit(m_group);
                if (ret < 0) {
                    g_logger->log(ERR, std::format("Failed to commit entry group: {}", avahi_strerror(ret)));
                    return shutdown();
                }
            }
        }

        void client_callback(AvahiClient* c, AvahiClientState state) {
            if (!m_client)
                m_client = c;

            switch (state) {
                case AVAHI_CLIENT_S_RUNNING: create_service(); break;
                case AVAHI_CLIENT_S_COLLISION:
                case AVAHI_CLIENT_S_REGISTERING:
                    if (m_group)
                        avahi_entry_group_reset(m_group);
                    break;
                case AVAHI_CLIENT_FAILURE: {
                    g_logger->log(ERR, std::format("Client failed with error: {}", avahi_strerror(avahi_client_errno(m_client))));
                    return shutdown();
                }
                case AVAHI_CLIENT_CONNECTING: break;
            }
        }

        void group_callback(AvahiEntryGroup* g, AvahiEntryGroupState state) {
            assert(m_group == nullptr || m_group == g);
            m_group = g;

            switch (state) {
                case AVAHI_ENTRY_GROUP_ESTABLISHED:
                    g_logger->log(INFO, std::format("Service '{}' successfully established on port '{}'", m_service_name, m_port));
                    break;

                case AVAHI_ENTRY_GROUP_COLLISION: {
                    alternative_service_name();
                    g_logger->log(WARN, std::format("Service name collision, renaming service to '{}'", m_service_name));

                    if (m_group)
                        avahi_entry_group_reset(m_group);

                    create_service();
                    break;
                }

                case AVAHI_ENTRY_GROUP_FAILURE: {
                    g_logger->log(ERR, std::format("Entry group failed with error: {}", avahi_strerror(avahi_client_errno(m_client))));
                    return shutdown();
                }
                case AVAHI_ENTRY_GROUP_UNCOMMITED:
                case AVAHI_ENTRY_GROUP_REGISTERING:;
            }
        }

        void alternative_service_name() {
            m_service_name = std::format("{} #{}", BASE_SERVICE_NAME, ++m_service_count);
        }

      private:
        AvahiSimplePoll* m_simple_poll{nullptr};
        AvahiClient*     m_client{nullptr};
        AvahiEntryGroup* m_group{nullptr};
        std::string      m_service_name{BASE_SERVICE_NAME};
        size_t           m_service_count{0};
        const int        m_port;
    };

    class CAutoRestartableAvahiService {
      public:
        CAutoRestartableAvahiService() {}

        void start(std::optional<int> port) {
            m_port = port;
            while (!m_shutdown.load(std::memory_order::relaxed)) {
                reset();
                g_logger->log(DEBUG, "Autorestrtable started");

                if (port.has_value()) {
                    m_avahi_service = new CAvahiService{port.value()};
                } else {
                    m_avahi_service = new CAvahiService{};
                }

                auto _ret = m_avahi_service->start_and_wait();
            }
        }

        void shutdown() {
            m_shutdown.store(true, std::memory_order::relaxed);
            if (m_avahi_service)
                m_avahi_service->shutdown();
        }

        void reset() {
            if (m_avahi_service) {
                m_avahi_service->shutdown();
                delete m_avahi_service;
                m_avahi_service = nullptr;
            }
        }

      private:
        std::atomic<bool>  m_shutdown{false};
        CAvahiService*     m_avahi_service{nullptr};
        std::optional<int> m_port{std::nullopt};
    };
}

#endif // !MDNS_AVAHI_SERVICE_REGISTRATION
