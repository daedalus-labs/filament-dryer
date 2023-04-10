/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include "gpio.hpp"

#include <cfloat>
#include <climits>
#include <cstdint>


inline constexpr float DEFAULT_TEMPERATURE = FLT_MIN;
inline constexpr float MINIMUM_TARGET_TEMPERATURE = FLT_MIN;
inline constexpr float MINIMUM_HYSTERESIS = 1.0f;
inline constexpr uint32_t MINIMUM_OFF_TIME_MS = 30 * 1000;
