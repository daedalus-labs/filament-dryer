/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include "gpio.hpp"

#include <cstdint>
#include <string_view>


inline constexpr uint32_t WIFI_TIMEOUT_MS = 30000;
inline constexpr uint32_t POLL_WAIT_TIME_MS = 2000;
inline constexpr std::string_view WIFI_STATUS_CONNECTED = "CONNECTED";
inline constexpr std::string_view WIFI_STATUS_CONNECTING = "CONNECTING";
inline constexpr std::string_view WIFI_STATUS_CONNECTED_NO_IP = "CONNECTED_NO_IP";
inline constexpr std::string_view WIFI_STATUS_NOT_CONNECTED = "NOT_CONNECTED";
inline constexpr std::string_view WIFI_STATUS_FAILURE = "FAILURE";
inline constexpr std::string_view WIFI_STATUS_SSID_NOT_FOUND = "SSID_NOT_FOUND";
inline constexpr std::string_view WIFI_STATUS_AUTHENTICATION_FAILURE = "AUTHENTICATION_FAILURE";
inline constexpr std::string_view WIFI_STATUS_UNKNOWN = "UNKNOWN";
inline constexpr std::string_view UNKNOWN_IP = "-";

namespace mqtt {
inline constexpr uint16_t KEEP_ALIVE_TIMEOUT_S = 120;
inline constexpr uint8_t CONNECTED = ON;
inline constexpr uint8_t DISCONNECTED = OFF;
} // namespace mqtt
