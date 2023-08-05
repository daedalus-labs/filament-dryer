/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include "connectivity/wireless/connection-status.hpp"

#include <lwip/netif.h>

#include <array>
#include <cstdint>
#include <string>


struct WifiState; // Forward declaration of WifiState

using MACAddress = std::array<uint8_t, 6>;

/**
 * An encapsulation of a connection to a wireless network.
 */
class WifiConnection
{
public:
    /**
     * Initializes a wireless connection to a wireless network.
     *
     * @param[in] hostname The Hostname to be used when connecting to @a ssid.
     * @param[in] ssid The SSID of the network to connect to.
     */
    WifiConnection(const std::string &hostname, const std::string& ssid);

    /**
     * Initializes a wireless connection to a wireless network.
     *
     * @param[in] hostname The Hostname to be used when connecting to @a ssid.
     * @param[in] ssid The SSID of the network to connect to.
     * @param[in] passphrase The passphrase of the network to connect to.
     */
    WifiConnection(const std::string &hostname, const std::string& ssid, const std::string& passphrase);

    /** Destructor */
    ~WifiConnection();

    /**
     * @return The IP address of this connection (in IPv4 format).
     */
    std::string ipAddress() const;

    /**
     * @return The netmask of this connection (in IPv4 format).
     */
    std::string netmask() const;

    /**
     * @return The IP address of the gateway of this connection (in IPv4 format).
     */
    std::string gateway() const;

    /**
     * @return The MAC Address of the device hosting this connection.
     */
    const MACAddress& macAddress() const;

    /**
     * @return The current status of this connection.
     */
    ConnectionStatus status() const;

    /**
     * Requests this connection to perform any necessary maintenance tasks.
     *
     * This method should be called periodically from the main thread.
     */
    void poll();

    /**
     * Resets the connection to the SSID.
     */
    void reset();

private:
    /**
     * Helper method to connect to the defined wireless network.
     */
    void _connectToWireless();

    /**
     * Helper method to set the hostname of this device.
     */
    void _setHostname();

    MACAddress _address;
    std::string _hostname;
    std::string _ssid;
    std::string _passphrase;
    struct netif* _interface;
};
