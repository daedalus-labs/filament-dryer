/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#include "generated/configuration.hpp"
#include "sensors/board.hpp"
#include "sensors/constants.hpp"
#include "sensors/dht.hpp"
#include "utilities.hpp"

#include <hardware/adc.h>
#include <hardware/gpio.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>

#include <cstdint>
#include <cstdio>


int main(int argc, char** argv)
{
    stdio_init_all();
    adc_init();

    DHT sensor(DHTType::DHT22, DHT_DATA_PIN, DHT_FEEDBACK_PIN);
    sensors::Board board;
    uint32_t count = 0;
    float board_temp = 0.0f;
    float sensor_temp = 0.0f;
    sleep_ms(10000);

    while (true) {
        printf("\n----------------- [%u]\n", count);

        board_temp = board.temperature();
        sensor_temp = sensor.temperature();
        sensor.read();

        printf("Temperature: %.1fC (%.1fF), Humidity: %.1f%%\n", sensor_temp, toFahrenheit(sensor_temp), sensor.humidity());
        printf("CPU Temperature: %.1fC (%.1fF)\n", board_temp, toFahrenheit(board_temp));

        printf("-----------------\n");

        sleep_ms(10000);
        count++;
    }

    return 0;
}
