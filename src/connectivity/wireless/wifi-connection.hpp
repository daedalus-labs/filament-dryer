/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include "connectivity/wireless/connection-status.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <string_view>


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
     * @param[in] ssid The SSID of the network to connect to.
     */
    WifiConnection(std::string_view ssid);

    /**
     * Initializes a wireless connection to a wireless network.
     *
     * @param[in] ssid The SSID of the network to connect to.
     * @param[in] passphrase The passphrase of the network to connect to.
     */
    WifiConnection(std::string_view ssid, std::string_view passphrase);

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
     * Requests this connection to perform any necessary maintenance tasks on this connection.
     *
     * This method should be called periodically from the main thread.
     */
    void poll();

private:
    /**
     * Helper method to connect to the defined wireless network.
     *
     * @param[in] ssid The SSID of the network to connect to.
     * @param[in] passphrase The passphrase of the network to connect to.
     */
    void _connectToWireless(std::string_view ssid, std::string_view passphrase);

    WifiState* _state;
    MACAddress _address;
};
