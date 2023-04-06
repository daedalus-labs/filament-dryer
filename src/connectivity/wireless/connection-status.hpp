/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include <cstdint>
#include <string_view>

/** Enumeration of the possible status values of a Wireless Connection */
enum class ConnectionStatus : uint8_t
{
    CONNECTED = 0x00,
    WIFI_CONNECTING,
    CONNECTING_TO_SERVER,
    INITIALIZATION_FAILURE = 0xC0,
    WIFI_CONNECTION_FAILURE,
    SERVER_CONNECTION_FAILURE
};

/**
 * Converts @a status to a human readable string.
 *
 * @param[in] status The ConnectionStatus value.
 * @return A human readable name for @a status.
 */
std::string_view toString(ConnectionStatus status);
