/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once


#include <pico/time.h>
#include <pico/types.h>

#include <cstdint>


inline constexpr float C_TO_F_SCALE = 9.0 / 5.0;
inline constexpr float C_TO_F_OFFSET = 32.0;

/**
 * @return The microseconds since boot.
 */
uint64_t microseconds();

/**
 * @return The milliseconds since boot.
 */
uint64_t milliseconds();

uint64_t systemIdentifier();

/**
 * Waits on the to become @a desired_state.
 *
 * Will timeout after @a wait_length, and return false.
 *
 * @note This is a blocking wait.
 * @param[in] gpio_pin The pin to wait on.
 * @param[in] desired_state The desired state of @a gpio_pin.
 * @param[in] wait_length The time to wait for it to enter this state in microseconds (us).
 * @return True if the wait was successful, false otherwise.
 */
bool wait(uint8_t gpio_pin, bool desired_state, uint64_t wait_length);

/**
 * Converts a temperature in celsius to its equivalent in fahrenheit.
 *
 * @param[in] temperature The temperature in celsius.
 * @return The value of @a temperature converted to fahrenheit.
 */
inline float toFahrenheit(float temperature)
{
    return (C_TO_F_SCALE * temperature) + C_TO_F_OFFSET;
}
