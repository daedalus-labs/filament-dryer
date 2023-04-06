/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/

#include "connectivity/wireless/connection-status.hpp"

#include "connectivity/constants.hpp"

#include <string_view>


std::string_view toString(ConnectionStatus status)
{
    switch (status) {
    case ConnectionStatus::CONNECTED:
        return WIFI_STATUS_CONNECTED;
    case ConnectionStatus::WIFI_CONNECTING:
        return WIFI_STATUS_WIFI_CONNECTING;
    case ConnectionStatus::CONNECTING_TO_SERVER:
        return WIFI_STATUS_CONNECTING_TO_SERVER;
    case ConnectionStatus::WIFI_CONNECTION_FAILURE:
        return WIFI_STATUS_WIFI_CONNECTION_FAILURE;
    case ConnectionStatus::INITIALIZATION_FAILURE:
        return WIFI_STATUS_INITIALIZATION_FAILURE;
    case ConnectionStatus::SERVER_CONNECTION_FAILURE:
        return WIFI_STATUS_SERVER_CONNECTION_FAILURE;
    default:
        return WIFI_STATUS_UNKNOWN;
    }
}
