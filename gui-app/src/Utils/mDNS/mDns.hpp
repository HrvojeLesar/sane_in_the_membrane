#ifndef UTILS_MDNS_MDNS
#define UTILS_MDNS_MDNS

#include <expected>
#include <format>
#include <vector>
#include <string>
extern "C" {
#include <mdns.h>
}

namespace sane_in_the_membrane::utils::mdns {
    struct SQueryResult {
        enum EAddressType {
            IPV4,
            IPV6
        };
        std::string  address;
        uint16_t     port;
        EAddressType address_type;

        std::string  as_address_with_port() const {
            return std::format("{}:{}", address, port);
        }
    };

    struct SmDNSOptions {
        long discover_timeout_sec;
        long query_timeout_sec;
        bool continue_after_discover;
        bool continue_after_query;
    };

    // Most implementation from mdns.c, credits to https://github.com/mjansson/mdns
    class CMDns {
        static constexpr char EXPECTED_ENTRY_TYPE_NAME[]{"_services._dns-sd._udp.local."};
        static constexpr char SERVICE_NAME[]{"_sane_in_the_membrane_service"};

        class CPartialQueryResult {
          public:
            void                        set_ipv4(std::string&& addr);
            void                        set_ipv6(std::string&& addr);
            void                        set_port(uint16_t port);

            std::optional<SQueryResult> build_ipv4();
            std::optional<SQueryResult> build_ipv6();

            void                        reset();

          private:
            std::optional<std::string> m_ip_v4_addr{std::nullopt};
            std::optional<std::string> m_ip_v6_addr{std::nullopt};
            std::optional<uint16_t>    m_port{std::nullopt};
        };

        struct SMDNSQuery {
            std::string        name;
            mdns_record_type_t type;
        };

        static int s_discover_callback(int sock, const struct sockaddr* from, size_t addrlen, mdns_entry_type_t entry, uint16_t query_id, uint16_t rtype, uint16_t rclass,
                                       uint32_t ttl, const void* data, size_t size, size_t name_offset, size_t name_length, size_t record_offset, size_t record_length,
                                       void* user_data) {
            auto mdnsclass = static_cast<CMDns*>(user_data);

            return mdnsclass->discover_callback(sock, from, addrlen, entry, query_id, rtype, rclass, ttl, data, size, name_offset, name_length, record_offset, record_length);
        }

        static int s_query_callback(int sock, const struct sockaddr* from, size_t addrlen, mdns_entry_type_t entry, uint16_t query_id, uint16_t rtype, uint16_t rclass,
                                    uint32_t ttl, const void* data, size_t size, size_t name_offset, size_t name_length, size_t record_offset, size_t record_length,
                                    void* user_data) {
            auto mdnsclass = static_cast<CMDns*>(user_data);

            return mdnsclass->query_callback(sock, from, addrlen, entry, query_id, rtype, rclass, ttl, data, size, name_offset, name_length, record_offset, record_length);
        }

      public:
        CMDns(long discover_timeout_sec = 5, long query_timeout_sec = 10);
        CMDns(SmDNSOptions& options);

        std::expected<void, std::string>                      discover_services(void);
        std::expected<std::vector<SQueryResult>, std::string> query_services();

      private:
        // Reimplementation of open_client_sockets from <mdns.c>, credits to https://github.com/mjansson/mdns
        int open_client_sockets(int* sockets, int max_sockets, int port);
        int discover_callback(int sock, const struct sockaddr* from, size_t addrlen, mdns_entry_type_t entry, uint16_t query_id, uint16_t rtype, uint16_t rclass, uint32_t ttl,
                              const void* data, size_t size, size_t name_offset, size_t name_length, size_t record_offset, size_t record_length);
        int query_callback(int sock, const struct sockaddr* from, size_t addrlen, mdns_entry_type_t entry, uint16_t query_id, uint16_t rtype, uint16_t rclass, uint32_t ttl,
                           const void* data, size_t size, size_t name_offset, size_t name_length, size_t record_offset, size_t record_length);
        std::expected<void, std::string> send_mdns_query(mdns_query_t* query, size_t count);
        mdns_string_t                    ipv4_address_to_string(char* buffer, size_t capacity, const struct sockaddr_in* addr, size_t addrlen);
        mdns_string_t                    ipv6_address_to_string(char* buffer, size_t capacity, const struct sockaddr_in6* addr, size_t addrlen);
        mdns_string_t                    ip_address_to_string(char* buffer, size_t capacity, const struct sockaddr* addr, size_t addrlen);

      private:
        int                       m_has_ipv4{};
        int                       m_has_ipv6{};

        sockaddr_in               m_service_address_ipv4{};
        sockaddr_in6              m_service_address_ipv6{};

        char                      m_entry_name_buffer[256]{};
        char                      m_general_use_buffer[256]{};
        std::vector<SMDNSQuery>   m_advertised_services{};

        CPartialQueryResult       m_partial_query_result{};
        std::vector<SQueryResult> m_query_results{};

        const SmDNSOptions        m_options;
    };
}

#endif // !UTILS_MDNS_MDNS
