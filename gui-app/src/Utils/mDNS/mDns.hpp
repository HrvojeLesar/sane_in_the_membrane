#ifndef UTILS_MDNS_MDNS
#define UTILS_MDNS_MDNS

#include <cstring>
#include <expected>
#include <optional>
#include <ranges>
#include <string>
#include <vector>
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include <stdio.h>

#include <errno.h>

#ifdef _WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#else
#include <netdb.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/time.h>
#endif

extern "C" {
#include <mdns.h>
}

#include "../../GlobalLogger.cpp"

namespace sane_in_the_membrane::utils::mdns {
    // Most implementation from mdns.c, credits to https://github.com/mjansson/mdns
    class CMDns {

        struct SQueryResult {
            enum EAddressType {
                IPV4,
                IPV6
            };
            std::string  address;
            uint16_t     port;
            EAddressType address_type;
        };

        class CPartialQueryResult {
          public:
            void set_ipv4(std::string&& addr) {
                m_ip_v4_addr = std::move(addr);
            }
            void set_ipv6(std::string&& addr) {
                m_ip_v6_addr = std::move(addr);
            }
            void set_port(uint16_t port) {
                m_port = port;
            }

            std::optional<SQueryResult> build_ipv4() {
                if (!m_ip_v4_addr.has_value())
                    return std::nullopt;
                if (!m_port.has_value())
                    return std::nullopt;

                auto result = SQueryResult{
                    .address      = std::move(m_ip_v4_addr).value(),
                    .port         = m_port.value(),
                    .address_type = SQueryResult::EAddressType::IPV4,
                };
                reset();

                return result;
            }

            std::optional<SQueryResult> build_ipv6() {
                if (!m_ip_v6_addr.has_value())
                    return std::nullopt;
                if (!m_port.has_value())
                    return std::nullopt;

                auto result = SQueryResult{
                    .address      = std::move(m_ip_v6_addr).value(),
                    .port         = m_port.value(),
                    .address_type = SQueryResult::EAddressType::IPV6,
                };
                reset();

                return result;
            }

            void reset() {
                m_ip_v4_addr = std::nullopt;
                m_ip_v6_addr = std::nullopt;
                m_port       = std::nullopt;
            }

          private:
            std::optional<std::string> m_ip_v4_addr{std::nullopt};
            std::optional<std::string> m_ip_v6_addr{std::nullopt};
            std::optional<uint16_t>    m_port{std::nullopt};
        };

        struct SMDNSQuery {
            std::string        name;
            mdns_record_type_t type;
        };

        static constexpr char EXPECTED_ENTRY_TYPE_NAME[]{"_services._dns-sd._udp.local."};
        static constexpr char SERVICE_NAME[]{"_sane_in_the_membrane_service"};

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
        CMDns(long discover_timeout_sec = 5, long query_timeout_sec = 10) : m_discover_timeout_sec(discover_timeout_sec), m_query_timeout_sec(query_timeout_sec) {}

        std::expected<void, std::string> discover_services(void) {
            int sockets[32];
            int num_sockets = open_client_sockets(sockets, sizeof(sockets) / sizeof(sockets[0]), 0);
            if (num_sockets <= 0) {
                g_logger->log(ERR, "Failed to open any client sockets");
                return std::unexpected("Failed to open any client sockets");
            }

            g_logger->log(DEBUG, std::format("Opened {} socket{} for DNS-SD", num_sockets, num_sockets > 1 ? "s" : ""));

            g_logger->log(DEBUG, "Sending DNS-SD discovery");
            for (int isock = 0; isock < num_sockets; ++isock) {
                if (mdns_discovery_send(sockets[isock]))
                    g_logger->log(ERR, std::format("Failed to send DNS-DS discovery: {}", strerror(errno)));
            }

            size_t capacity = 2048;
            void*  buffer   = malloc(capacity);
            size_t records;

            int    res;
            g_logger->log(DEBUG, "Reading DNS-SD replies");
            do {
                struct timeval timeout;
                timeout.tv_sec  = m_discover_timeout_sec;
                timeout.tv_usec = 0;

                int    nfds = 0;
                fd_set readfs;
                FD_ZERO(&readfs);
                for (int isock = 0; isock < num_sockets; ++isock) {
                    if (sockets[isock] >= nfds)
                        nfds = sockets[isock] + 1;
                    FD_SET(sockets[isock], &readfs);
                }

                records = 0;
                res     = select(nfds, &readfs, 0, 0, &timeout);
                if (res > 0) {
                    for (int isock = 0; isock < num_sockets; ++isock) {
                        if (FD_ISSET(sockets[isock], &readfs)) {
                            records += mdns_discovery_recv(sockets[isock], buffer, capacity, s_discover_callback, this);
                        }
                    }
                }
            } while (res > 0);

            free(buffer);

            for (int isock = 0; isock < num_sockets; ++isock)
                mdns_socket_close(sockets[isock]);
            g_logger->log(INFO, std::format("Closed socket{}", num_sockets > 1 ? "s" : ""));

            return {};
        }

        std::expected<std::vector<SQueryResult>, std::string> query_services() {
            std::vector<mdns_query_t> queries = m_advertised_services | std::views::transform([](SMDNSQuery& query) {
                                                    return mdns_query_t{
                                                        .type   = query.type,
                                                        .name   = query.name.data(),
                                                        .length = query.name.size(),
                                                    };
                                                }) |
                std::ranges::to<std::vector>();

            auto result = send_mdns_query(queries.data(), queries.size());
            if (!result.has_value()) {
                return std::unexpected(std::move(result).error());
            }

            return std::move(m_query_results);
        }

      private:
        // Reimplementation of open_client_sockets from <mdns.c>, credits to https://github.com/mjansson/mdns
        int open_client_sockets(int* sockets, int max_sockets, int port) {
            int num_sockets = 0;

// TODO: Proper windows implementation
// TODO: Confirm it actually compiles
#ifdef _WIN32
            IP_ADAPTER_ADDRESSES* adapter_address = 0;
            ULONG                 address_size    = 8000;
            unsigned int          ret;
            unsigned int          num_retries = 4;
            do {
                adapter_address = (IP_ADAPTER_ADDRESSES*)malloc(address_size);
                ret             = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_ANYCAST, 0, adapter_address, &address_size);
                if (ret == ERROR_BUFFER_OVERFLOW) {
                    free(adapter_address);
                    adapter_address = 0;
                    address_size *= 2;
                } else {
                    break;
                }
            } while (num_retries-- > 0);

            if (!adapter_address || (ret != NO_ERROR)) {
                free(adapter_address);
                g_logger->log(ERR, "Failed to get network adapter addresses");
                return num_sockets;
            }

            int first_ipv4 = 1;
            int first_ipv6 = 1;
            for (PIP_ADAPTER_ADDRESSES adapter = adapter_address; adapter; adapter = adapter->Next) {
                if (adapter->TunnelType == TUNNEL_TYPE_TEREDO)
                    continue;
                if (adapter->OperStatus != IfOperStatusUp)
                    continue;

                for (IP_ADAPTER_UNICAST_ADDRESS* unicast = adapter->FirstUnicastAddress; unicast; unicast = unicast->Next) {
                    if (unicast->Address.lpSockaddr->sa_family == AF_INET) {
                        struct sockaddr_in* saddr = (struct sockaddr_in*)unicast->Address.lpSockaddr;
                        if ((saddr->sin_addr.S_un.S_un_b.s_b1 != 127) || (saddr->sin_addr.S_un.S_un_b.s_b2 != 0) || (saddr->sin_addr.S_un.S_un_b.s_b3 != 0) ||
                            (saddr->sin_addr.S_un.S_un_b.s_b4 != 1)) {
                            if (first_ipv4) {
                                m_service_address_ipv4 = *saddr;
                                first_ipv4             = 0;
                            }
                            m_has_ipv4 = 1;
                            if (num_sockets < max_sockets) {
                                saddr->sin_port = htons((unsigned short)port);
                                int sock        = mdns_socket_open_ipv4(saddr);
                                if (sock >= 0) {
                                    sockets[num_sockets++] = sock;
                                }
                            }
                        }
                    } else if (unicast->Address.lpSockaddr->sa_family == AF_INET6) {
                        struct sockaddr_in6* saddr = (struct sockaddr_in6*)unicast->Address.lpSockaddr;
                        // Ignore link-local addresses
                        if (saddr->sin6_scope_id)
                            continue;
                        static const unsigned char localhost[]        = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
                        static const unsigned char localhost_mapped[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 0x7f, 0, 0, 1};
                        if ((unicast->DadState == NldsPreferred) && memcmp(saddr->sin6_addr.s6_addr, localhost, 16) && memcmp(saddr->sin6_addr.s6_addr, localhost_mapped, 16)) {
                            if (first_ipv6) {
                                m_service_address_ipv6 = *saddr;
                                first_ipv6             = 0;
                            }
                            m_has_ipv6 = 1;
                            if (num_sockets < max_sockets) {
                                saddr->sin6_port = htons((unsigned short)port);
                                int sock         = mdns_socket_open_ipv6(saddr);
                                if (sock >= 0) {
                                    sockets[num_sockets++] = sock;
                                }
                            }
                        }
                    }
                }
            }

            free(adapter_address);

#else

            struct ifaddrs* ifaddr = 0;
            struct ifaddrs* ifa    = 0;

            if (getifaddrs(&ifaddr) < 0)
                g_logger->log(ERR, "Unable to get interface addresses");

            int first_ipv4 = 1;
            int first_ipv6 = 1;
            for (ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
                if (!ifa->ifa_addr)
                    continue;
                if (!(ifa->ifa_flags & IFF_UP) || !(ifa->ifa_flags & IFF_MULTICAST))
                    continue;
                if ((ifa->ifa_flags & IFF_LOOPBACK) || (ifa->ifa_flags & IFF_POINTOPOINT))
                    continue;

                if (ifa->ifa_addr->sa_family == AF_INET) {
                    struct sockaddr_in* saddr = (struct sockaddr_in*)ifa->ifa_addr;
                    if (saddr->sin_addr.s_addr != htonl(INADDR_LOOPBACK)) {
                        if (first_ipv4) {
                            m_service_address_ipv4 = *saddr;
                            first_ipv4             = 0;
                        }
                        m_has_ipv4 = 1;
                        if (num_sockets < max_sockets) {
                            saddr->sin_port = htons(port);
                            int sock        = mdns_socket_open_ipv4(saddr);
                            if (sock >= 0) {
                                sockets[num_sockets++] = sock;
                            }
                        }
                    }
                } else if (ifa->ifa_addr->sa_family == AF_INET6) {
                    struct sockaddr_in6* saddr = (struct sockaddr_in6*)ifa->ifa_addr;
                    if (saddr->sin6_scope_id)
                        continue;
                    constexpr unsigned char localhost[]        = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
                    constexpr unsigned char localhost_mapped[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 0x7f, 0, 0, 1};
                    if (memcmp(saddr->sin6_addr.s6_addr, localhost, 16) && memcmp(saddr->sin6_addr.s6_addr, localhost_mapped, 16)) {
                        if (first_ipv6) {
                            m_service_address_ipv6 = *saddr;
                            first_ipv6             = 0;
                        }
                        m_has_ipv6 = 1;
                        if (num_sockets < max_sockets) {
                            saddr->sin6_port = htons(port);
                            int sock         = mdns_socket_open_ipv6(saddr);
                            if (sock >= 0) {
                                sockets[num_sockets++] = sock;
                            }
                        }
                    }
                }
            }

            freeifaddrs(ifaddr);

#endif

            return num_sockets;
        }

        int discover_callback(int sock, const struct sockaddr* from, size_t addrlen, mdns_entry_type_t entry, uint16_t query_id, uint16_t rtype, uint16_t rclass, uint32_t ttl,
                              const void* data, size_t size, size_t name_offset, size_t name_length, size_t record_offset, size_t record_length) {
            mdns_string_extract(data, size, &name_offset, m_entry_name_buffer, sizeof(m_entry_name_buffer));

            if (strcmp(m_entry_name_buffer, EXPECTED_ENTRY_TYPE_NAME) == 0) {
                std::string service_name{};
                service_name.resize(size);

                auto mdns_string_data = mdns_record_parse_ptr(data, size, record_offset, record_length, service_name.data(), service_name.capacity());
                service_name.resize(mdns_string_data.length);

                if (rtype == MDNS_RECORDTYPE_PTR && service_name.contains(SERVICE_NAME)) {
                    m_advertised_services.push_back(SMDNSQuery{
                        .name = std::move(service_name),
                        .type = static_cast<mdns_record_type_t>(rtype),
                    });
                }
            }

            return 0;
        }

        int query_callback(int sock, const struct sockaddr* from, size_t addrlen, mdns_entry_type_t entry, uint16_t query_id, uint16_t rtype, uint16_t rclass, uint32_t ttl,
                           const void* data, size_t size, size_t name_offset, size_t name_length, size_t record_offset, size_t record_length) {
            if (rtype == MDNS_RECORDTYPE_SRV) {
                mdns_record_srv_t srv = mdns_record_parse_srv(data, size, record_offset, record_length, m_entry_name_buffer, sizeof(m_entry_name_buffer));
                m_partial_query_result.set_port(srv.port);
            } else if (rtype == MDNS_RECORDTYPE_A) {
                struct sockaddr_in addr;
                mdns_record_parse_a(data, size, record_offset, record_length, &addr);
                mdns_string_t addrstr = ipv4_address_to_string(m_general_use_buffer, sizeof(m_general_use_buffer), &addr, sizeof(addr));
                m_partial_query_result.set_ipv4(std::string{addrstr.str, addrstr.length});
            } else if (rtype == MDNS_RECORDTYPE_AAAA) {
                struct sockaddr_in6 addr;
                mdns_record_parse_aaaa(data, size, record_offset, record_length, &addr);
                mdns_string_t addrstr = ipv6_address_to_string(m_general_use_buffer, sizeof(m_general_use_buffer), &addr, sizeof(addr));
                m_partial_query_result.set_ipv6(std::string{addrstr.str, addrstr.length});
            }

            auto query_addr = m_partial_query_result.build_ipv4();
            if (query_addr.has_value()) {
                m_query_results.push_back(std::move(query_addr).value());
            }

            return 0;
        }

        std::expected<void, std::string> send_mdns_query(mdns_query_t* query, size_t count) {
            int sockets[32];
            int query_id[32];
            int num_sockets = open_client_sockets(sockets, sizeof(sockets) / sizeof(sockets[0]), 0);
            if (num_sockets <= 0) {
                g_logger->log(ERR, "Failed to open any client sockets");
                return std::unexpected("Failed to open any client sockets");
            }
            g_logger->log(DEBUG, std::format("Opened {} socket{} for mDNS query", num_sockets, num_sockets > 1 ? "s" : ""));

            size_t capacity = 2048;
            void*  buffer   = malloc(capacity);

            for (size_t iq = 0; iq < count; ++iq) {
                const char* record_name = "PTR";
                if (query[iq].type == MDNS_RECORDTYPE_SRV)
                    record_name = "SRV";
                else if (query[iq].type == MDNS_RECORDTYPE_A)
                    record_name = "A";
                else if (query[iq].type == MDNS_RECORDTYPE_AAAA)
                    record_name = "AAAA";
                else
                    query[iq].type = MDNS_RECORDTYPE_PTR;
                g_logger->log(DEBUG, std::format("Sending mDNS query: {} {}", query[iq].name, record_name));
            }

            for (int isock = 0; isock < num_sockets; ++isock) {
                query_id[isock] = mdns_multiquery_send(sockets[isock], query, count, buffer, capacity, 0);
                if (query_id[isock] < 0)
                    g_logger->log(ERR, std::format("Failed to send mDNS query: {}", strerror(errno)));
            }

            int res;
            printf("Reading mDNS query replies\n");
            int records = 0;
            do {
                struct timeval timeout;
                timeout.tv_sec  = m_query_timeout_sec;
                timeout.tv_usec = 0;

                int    nfds = 0;
                fd_set readfs;
                FD_ZERO(&readfs);
                for (int isock = 0; isock < num_sockets; ++isock) {
                    if (sockets[isock] >= nfds)
                        nfds = sockets[isock] + 1;
                    FD_SET(sockets[isock], &readfs);
                }

                res = select(nfds, &readfs, 0, 0, &timeout);
                if (res > 0) {
                    for (int isock = 0; isock < num_sockets; ++isock) {
                        if (FD_ISSET(sockets[isock], &readfs)) {
                            size_t rec = mdns_query_recv(sockets[isock], buffer, capacity, s_query_callback, this, query_id[isock]);
                            if (rec > 0)
                                records += rec;
                        }
                        FD_SET(sockets[isock], &readfs);
                    }
                }
            } while (res > 0);

            free(buffer);

            for (int isock = 0; isock < num_sockets; ++isock)
                mdns_socket_close(sockets[isock]);

            return {};
        }

        mdns_string_t ipv4_address_to_string(char* buffer, size_t capacity, const struct sockaddr_in* addr, size_t addrlen) {
            char host[NI_MAXHOST]    = {0};
            char service[NI_MAXSERV] = {0};
            int  ret                 = getnameinfo((const struct sockaddr*)addr, (socklen_t)addrlen, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV | NI_NUMERICHOST);
            int  len                 = 0;
            if (ret == 0) {
                if (addr->sin_port != 0)
                    len = snprintf(buffer, capacity, "%s:%s", host, service);
                else
                    len = snprintf(buffer, capacity, "%s", host);
            }
            if (len >= (int)capacity)
                len = (int)capacity - 1;
            mdns_string_t str;
            str.str    = buffer;
            str.length = len;
            return str;
        }

        mdns_string_t ipv6_address_to_string(char* buffer, size_t capacity, const struct sockaddr_in6* addr, size_t addrlen) {
            char host[NI_MAXHOST]    = {0};
            char service[NI_MAXSERV] = {0};
            int  ret                 = getnameinfo((const struct sockaddr*)addr, (socklen_t)addrlen, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV | NI_NUMERICHOST);
            int  len                 = 0;
            if (ret == 0) {
                if (addr->sin6_port != 0)
                    len = snprintf(buffer, capacity, "[%s]:%s", host, service);
                else
                    len = snprintf(buffer, capacity, "%s", host);
            }
            if (len >= (int)capacity)
                len = (int)capacity - 1;
            mdns_string_t str;
            str.str    = buffer;
            str.length = len;
            return str;
        }

        mdns_string_t ip_address_to_string(char* buffer, size_t capacity, const struct sockaddr* addr, size_t addrlen) {
            if (addr->sa_family == AF_INET6)
                return ipv6_address_to_string(buffer, capacity, (const struct sockaddr_in6*)addr, addrlen);
            return ipv4_address_to_string(buffer, capacity, (const struct sockaddr_in*)addr, addrlen);
        }

      private:
        int                       m_has_ipv4{};
        int                       m_has_ipv6{};

        sockaddr_in               m_service_address_ipv4{};
        sockaddr_in6              m_service_address_ipv6{};

        char                      m_entry_name_buffer[256]{};
        char                      m_general_use_buffer[256]{};
        std::vector<SMDNSQuery>   m_advertised_services{};

        long                      m_discover_timeout_sec;
        long                      m_query_timeout_sec;
        CPartialQueryResult       m_partial_query_result{};
        std::vector<SQueryResult> m_query_results{};
    };
}

#endif // !UTILS_MDNS_MDNS
