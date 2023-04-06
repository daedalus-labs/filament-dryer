/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause

Based on code Copyright (c) 2023 Erich Styger
Original code is BSD-3-Clause
------------------------------------------------------------------------------*/

#include "connectivity/dns/resolver.hpp"

#include <lwip/dns.h>
#include <lwip/ip_addr.h>
#include <pico/cyw43_arch.h>

#include <cstdint>
#include <string_view>


namespace dns {
inline constexpr uint32_t TICK_PERIOD_MS = 10;

struct Request
{
    ip_addr_t resolved_address;
    bool sent;
    bool received;
    bool successful;
};

void onDNSRequest(const char* hostname, const ip_addr_t* ipaddr, void* arg)
{
    Request* info = static_cast<Request*>(arg);
    info->received = true;

    if (ipaddr == NULL) {
        printf("DNS request for %s failed\n", hostname);
        return;
    }

    info->resolved_address = *ipaddr;
    info->successful = true;
    printf("%s resolved to %s\n", hostname, ip4addr_ntoa(ipaddr));
}

bool resolve(const std::string& hostname, ip_addr_t& resolved_address, int32_t timeout_ms)
{
    // The hostname could just be an IP address.
    // Based on the lwIP documentation, if ip4addr_aton returns greater than 0, hostname
    // could be converted to an IP address without a DNS look-up.
    if (ip4addr_aton(hostname.c_str(), &resolved_address) > 0) {
        return true;
    }

    Request request;
    request.sent = true;
    request.received = false;
    request.successful = false;

    cyw43_arch_lwip_begin();
    err_t error = dns_gethostbyname(hostname.c_str(), &resolved_address, onDNSRequest, static_cast<void*>(&request));
    cyw43_arch_lwip_end();

    // ERR_OK is returned if the result has been cached.
    if (error == ERR_OK) {
        printf("Using cached request for %s", hostname.c_str());
        return true;
    }

    // ERR_INPROGRESS is returned if there is a pending DNS
    if (error != ERR_INPROGRESS) {
        printf("Failed to request DNS for %s: %s\n", hostname.c_str(), lwip_strerr(error));
        return false;
    }

    while (timeout_ms > 0 && !request.received) {
        sleep_until(make_timeout_time_ms(TICK_PERIOD_MS));
        timeout_ms -= TICK_PERIOD_MS;
    }

    if (timeout_ms <= 0) {
        printf("DNS request for %s timed out\n", hostname.c_str());
        return false;
    }

    if (!request.received) {
        printf("DNS response for '%s' not received\n", hostname.c_str());
        return false;
    }

    if (!request.successful) {
        printf("DNS response for '%s' received, but was not successful\n", hostname.c_str());
        return false;
    }

    // resolved_address above is only set if the result has been cached.
    resolved_address = request.resolved_address;
    return true;
}
} // namespace dns
