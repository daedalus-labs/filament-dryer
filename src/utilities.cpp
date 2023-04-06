/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/

#include "utilities.hpp"

#include <hardware/gpio.h>
#include <pico/time.h>
#include <pico/types.h>

#include <cstdint>


uint64_t microseconds()
{
    return to_us_since_boot(get_absolute_time());
}

bool wait(uint8_t gpio_pin, bool desired_state, uint64_t wait_length)
{
    uint64_t read_count = 0;
    while (gpio_get(gpio_pin) != desired_state && read_count < wait_length) {
        read_count += 10;
        sleep_us(10);
    }

    return read_count < wait_length;
}
