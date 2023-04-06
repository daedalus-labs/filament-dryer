/*------------------------------------------------------------------------------
Copyright 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#include "sensors/board.hpp"

#include "sensors/constants.hpp"

#include <hardware/adc.h>

#include <cstddef>
#include <cstdint>


namespace sensors {
inline constexpr uint8_t CPU_TEMP_ADC_PIN = 4;

Board::Board()
{}

float Board::temperature() const
{
    adc_set_temp_sensor_enabled(true);
    adc_select_input(CPU_TEMP_ADC_PIN);
    float adc = adc_read() * ADC_CONVERSION_FACTOR;
    return 27.0f - (adc - 0.706f) / 0.001721f;
}
} // namespace sensors
