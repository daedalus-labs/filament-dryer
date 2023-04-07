/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#include "connectivity/mqtt.hpp"
#include "connectivity/wireless.hpp"
#include "controllers/heater.hpp"
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


inline constexpr float HEATER_HYSTERESIS = 5.0f;
inline constexpr uint32_t HEATER_MAX_ON_TIME_S = 10 * 60;

int main(int argc, char** argv)
{
    stdio_init_all();
    adc_init();

    WifiConnection wifi(SSID, PASSPHRASE);
    mqtt::Client mqtt(MQTT_BROKER, CONFIGURED_MQTT_PORT, DEVICE_NAME);
    DHT sensor(DHTType::DHT22, DHT_DATA_PIN, DHT_FEEDBACK_PIN);
    sensors::Board board;
    controllers::Heater heater(HEATER_CONTROL_PIN, HEATER_FEEDBACK_PIN, HEATER_HYSTERESIS, HEATER_MAX_ON_TIME_S);

    uint32_t count = 0;
    float board_temp = 0.0f;
    float sensor_temp = 0.0f;

    sleep_ms(10000);

    while (true) {
        if (!mqtt.connected()) {
            printf("MQTT not connected\n");
            mqtt.connect();
            sleep_ms(17500);
            continue;
        }

        printf("\n----------------- [%u]\n", count);

        board_temp = board.temperature();
        sensor_temp = sensor.temperature();
        sensor.read();
        wifi.poll();

        printf("Temperature: %.1fC (%.1fF), Humidity: %.1f%%\n", sensor_temp, toFahrenheit(sensor_temp), sensor.humidity());
        printf("Wifi Connection Status: %s (%s)\n", toString(wifi.status()).data(), wifi.ipAddress().c_str());
        printf("CPU Temperature: %.1fC (%.1fF)\n", board_temp, toFahrenheit(board_temp));
        printf("MQTT Status: %s\n", mqtt.connected() ? "true" : "false");

        if (!mqtt.publish(VERSION_TOPIC, static_cast<const void*>(VERSION.data()), VERSION.size(), mqtt::QoS::EXACTLY_ONCE, true)) {
            printf("Failed to publish %s\n", VERSION_TOPIC.data());
        }

        printf("-----------------\n");

        sleep_ms(10000);
        count++;
    }

    return 0;
}
