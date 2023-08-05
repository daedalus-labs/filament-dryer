/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include <cstdint>
#include <string_view>

/** Enumeration of the possible status values of a Wireless Connection */
enum class ConnectionStatus : int8_t
{
    NOT_CONNECTED = 0,          ///< Link is down
    CONNECTING = 1,             ///< Connecting to Wifi.
    CONNECTED_NO_IP = 2,        ///< Connected to wifi, but no IP address
    CONNECTED = 3,              ///< Connected to wifi with an IP address
    FAILURE = -1,               ///< Connection failed
    SSID_NOT_FOUND = -2,        ///< No matching SSID found (could be out of range, or down)
    AUTHENTICATION_FAILURE = -3 ///< Authentication failure
};

/**
 * Converts @a status to a human readable string.
 *
 * @param[in] status The ConnectionStatus value.
 * @return A human readable name for @a status.
 */
std::string_view toString(ConnectionStatus status);
