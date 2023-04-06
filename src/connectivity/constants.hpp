/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/

#include <cstdint>
#include <string_view>


inline constexpr uint32_t WIFI_TIMEOUT_MS = 30000;
inline constexpr uint32_t POLL_WAIT_TIME_MS = 2000;
inline constexpr uint32_t POLL_TIME_S = 5;
inline constexpr std::string_view WIFI_STATUS_CONNECTED = "CONNECTED";
inline constexpr std::string_view WIFI_STATUS_WIFI_CONNECTING = "WIFI_CONNECTING";
inline constexpr std::string_view WIFI_STATUS_CONNECTING_TO_SERVER = "CONNECTING_TO_SERVER";
inline constexpr std::string_view WIFI_STATUS_INITIALIZATION_FAILURE = "INITIALIZATION_FAILURE";
inline constexpr std::string_view WIFI_STATUS_WIFI_CONNECTION_FAILURE = "WIFI_CONNECTION_FAILURE";
inline constexpr std::string_view WIFI_STATUS_SERVER_CONNECTION_FAILURE = "SERVER_CONNECTION_FAILURE";
inline constexpr std::string_view WIFI_STATUS_UNKNOWN = "UNKNOWN";
inline constexpr std::string_view UNKNOWN_IP = "-";

namespace mqtt {
inline constexpr uint16_t KEEP_ALIVE_TIMEOUT_S = 120;
inline constexpr uint8_t OFF = 0;
inline constexpr uint8_t ON = 1;
inline constexpr uint8_t CONNECTED = ON;
inline constexpr uint8_t DISCONNECTED = OFF;
} // namespace mqtt
