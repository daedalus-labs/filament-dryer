/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include <cfloat>
#include <climits>
#include <cstdint>


inline constexpr float DEFAULT_HUMIDITY = FLT_MIN;
inline constexpr float DEFAULT_TEMPERATURE = FLT_MIN;
inline constexpr uint32_t ON = 1;
inline constexpr uint32_t OFF = 0;
inline constexpr uint32_t HIGH = ON;
inline constexpr uint32_t LOW = OFF;
inline constexpr uint32_t BITS_IN_BYTE = (sizeof(uint8_t) * CHAR_BIT);
inline constexpr float FULL_BATTERY_VOLTAGE = 4.2f;
inline constexpr float EMPTY_BATTERY_VOLTAGE = 2.8f;
inline constexpr float ADC_VREF = 3.3f;
inline constexpr uint8_t ADC_RESOLUTION = 12;
inline constexpr float ADC_CONVERSION_FACTOR = ADC_VREF / (1 << ADC_RESOLUTION);
inline constexpr float VOLTAGE_DIVIDER_CONVERSION_FACTOR = 2.0f * ADC_CONVERSION_FACTOR;
inline constexpr uint8_t PERCENT_FACTOR = 100;
inline constexpr uint8_t GPIO_PIN_OFFSET = 26;
