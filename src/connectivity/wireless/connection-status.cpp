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
    case ConnectionStatus::CONNECTING:
        return WIFI_STATUS_CONNECTING;
    case ConnectionStatus::CONNECTED_NO_IP:
        return WIFI_STATUS_CONNECTED_NO_IP;
    case ConnectionStatus::FAILURE:
        return WIFI_STATUS_FAILURE;
    case ConnectionStatus::NOT_CONNECTED:
        return WIFI_STATUS_NOT_CONNECTED;
    case ConnectionStatus::AUTHENTICATION_FAILURE:
        return WIFI_STATUS_AUTHENTICATION_FAILURE;
    case ConnectionStatus::SSID_NOT_FOUND:
        return WIFI_STATUS_SSID_NOT_FOUND;
    default:
        return WIFI_STATUS_UNKNOWN;
    }
}
