/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/

#include "utilities.hpp"

#include "gpio.hpp"

#include <hardware/gpio.h>
#include <pico/time.h>
#include <pico/types.h>
#include <pico/unique_id.h>

#include <cstdint>
#include <cstring>


inline constexpr size_t UID_SIZE = 2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1;

uint64_t microseconds()
{
    return to_us_since_boot(get_absolute_time());
}

uint64_t milliseconds()
{
    return to_us_since_boot(get_absolute_time()) / 1000;
}

std::string systemIdentifier()
{
    char uid[UID_SIZE];
    std::memset(uid, 0, UID_SIZE);
    pico_get_unique_board_id_string(uid, UID_SIZE);
    return uid;
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
