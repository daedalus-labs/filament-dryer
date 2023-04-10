/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include "gpio.hpp"

#include <cfloat>
#include <climits>
#include <cstdint>


inline constexpr float DEFAULT_HUMIDITY = FLT_MIN;
inline constexpr float DEFAULT_TEMPERATURE = FLT_MIN;
inline constexpr float FULL_BATTERY_VOLTAGE = 4.2f;
inline constexpr float EMPTY_BATTERY_VOLTAGE = 2.8f;
inline constexpr uint8_t PERCENT_FACTOR = 100;
