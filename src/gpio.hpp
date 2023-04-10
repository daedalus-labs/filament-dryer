/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include <cfloat>
#include <climits>
#include <cstdint>


inline constexpr uint32_t ON = 1;
inline constexpr uint32_t OFF = 0;
inline constexpr uint32_t HIGH = ON;
inline constexpr uint32_t LOW = OFF;
inline constexpr uint32_t BITS_IN_BYTE = (sizeof(uint8_t) * CHAR_BIT);
inline constexpr uint8_t GPIO_PIN_OFFSET = 26;
inline constexpr float ADC_VREF = 3.3f;
inline constexpr uint8_t ADC_RESOLUTION = 12;
inline constexpr float ADC_CONVERSION_FACTOR = ADC_VREF / (1 << ADC_RESOLUTION);
inline constexpr float VOLTAGE_DIVIDER_CONVERSION_FACTOR = 2.0f * ADC_CONVERSION_FACTOR;