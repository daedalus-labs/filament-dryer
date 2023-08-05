/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#include "connectivity/wireless/wifi-connection.hpp"

#include "connectivity/constants.hpp"

#include <lwip/ip.h>
#include <lwip/netif.h>
#include <lwip/pbuf.h>
#include <lwip/tcp.h>
#include <pico/cyw43_arch.h>
#include <pico/error.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>

#include <cstdint>
#include <string>

inline constexpr uint32_t RECONNECT_TIMEOUT_MS = 60000;

/**
 * A RAII-style locking mechanism for the CYW43 lwIP stack.
 */
class WifiLock
{
public:
    WifiLock()
    {
        cyw43_arch_lwip_begin();
    }

    ~WifiLock()
    {
        cyw43_arch_lwip_end();
    }

private:
    WifiLock(const WifiLock&) = delete;
    WifiLock& operator=(const WifiLock&) = delete;
};

WifiConnection::WifiConnection(const std::string& hostname, const std::string& ssid) : WifiConnection(hostname, ssid, std::string())
{}

WifiConnection::WifiConnection(const std::string& hostname, const std::string& ssid, const std::string& passphrase)
    : _address(), _hostname(hostname), _ssid(ssid), _passphrase(passphrase), _interface(nullptr)
{
    _interface = &cyw43_state.netif[CYW43_ITF_STA];
    int32_t result = cyw43_arch_init_with_country(CYW43_COUNTRY_USA);
    if (result != PICO_OK) {
        printf("Failed to initialize Wifi: %d\n", result);
        return;
    }
    cyw43_arch_enable_sta_mode();

    _setHostname();

    uint8_t buffer[6];
    cyw43_hal_get_mac(0, buffer);
    for (size_t i = 0; i < _address.size(); i++) {
        _address[i] = buffer[i];
    }

    _connectToWireless();
}

WifiConnection::~WifiConnection()
{
    cyw43_arch_deinit();
}

std::string WifiConnection::ipAddress() const
{
    return ipaddr_ntoa(&_interface->ip_addr);
}

std::string WifiConnection::netmask() const
{
    return ipaddr_ntoa(&_interface->netmask);
}

std::string WifiConnection::gateway() const
{
    return ipaddr_ntoa(&_interface->gw);
}

const MACAddress& WifiConnection::macAddress() const
{
    return _address;
}

ConnectionStatus WifiConnection::status() const
{
    int32_t status = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
    switch (status) {
    case CYW43_LINK_JOIN:
        if (&_interface->ip_addr.addr > 0) {
            return ConnectionStatus::CONNECTED;
        }
        return ConnectionStatus::CONNECTING;
    case CYW43_LINK_UP:
        return ConnectionStatus::CONNECTED;
    case CYW43_LINK_NOIP:
        return ConnectionStatus::CONNECTED_NO_IP;
    case CYW43_LINK_NONET:
        return ConnectionStatus::SSID_NOT_FOUND;
    case CYW43_LINK_BADAUTH:
        return ConnectionStatus::AUTHENTICATION_FAILURE;
    case CYW43_LINK_FAIL:
        return ConnectionStatus::FAILURE;
    default:
        return ConnectionStatus::NOT_CONNECTED;
    }
}

void WifiConnection::poll()
{
    // This call is being used to maximize the windows of time in which this
    // application is available to do work for the CYW43.
    cyw43_arch_wait_for_work_until(make_timeout_time_ms(POLL_WAIT_TIME_MS));
}

void WifiConnection::reset()
{
    _connectToWireless();
}

void WifiConnection::_connectToWireless()
{
    WifiLock lock;
    printf("Connecting to Wifi (SSID: %s)...\n", _ssid.c_str());

    int32_t result = INT32_MIN;
    if (_passphrase.size() == 0) {
        result = cyw43_arch_wifi_connect_timeout_ms(_ssid.c_str(), NULL, CYW43_AUTH_OPEN, WIFI_TIMEOUT_MS);
    }
    else {
        result = cyw43_arch_wifi_connect_timeout_ms(_ssid.c_str(), _passphrase.c_str(), CYW43_AUTH_WPA2_AES_PSK, WIFI_TIMEOUT_MS);
    }

    if (result != PICO_OK) {
        printf("Failed to connect to %s: %d\n", _ssid.c_str(), result);
        return;
    }

    printf("Connected to %s\n", _ssid.c_str());
    netif_set_up(_interface);
}

void WifiConnection::_setHostname()
{
    WifiLock lock;
    printf("Setting hostname to %s\n", _hostname.c_str());
    netif_set_hostname(_interface, _hostname.c_str());
}
