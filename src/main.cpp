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
#include <pico/multicore.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <pico/util/queue.h>

#include <cstdint>
#include <cstdio>


inline constexpr float HEATER_HYSTERESIS = 5.0f;
inline constexpr uint64_t HEATER_MAX_ON_TIME_MS = 10 * 60 * 1000;
inline constexpr uint32_t DATA_PERIOD_MS = 10000;
inline constexpr uint32_t COMMUNICATION_PERIOD_MS = 10000;
inline constexpr uint32_t MQTT_CONNECTION_WAIT_MS = 17500;
inline constexpr uint8_t QUEUE_SIZE = 5;

typedef struct
{
    float board_temperature;
    float container_temperature;
    float container_humidity;
    float target_temperature;
    bool heater_on;
} feedback_entry;

typedef struct
{
    float target_temperature;
} request_entry;

queue_t feedback_queue;
queue_t request_queue;

void controlLoop()
{
    DHT sensor(DHTType::DHT22, DHT_DATA_PIN, DHT_FEEDBACK_PIN);
    sensors::Board board;
    controllers::Heater heater(HEATER_CONTROL_PIN, HEATER_FEEDBACK_PIN, HEATER_HYSTERESIS, HEATER_MAX_ON_TIME_MS);

    while (true) {
        if (!queue_is_empty(&request_queue)) {
            request_entry request;
            queue_remove_blocking(&request_queue, &request);
            heater.setTargetTemperature(request.target_temperature);
        }

        sensor.read();
        heater.update(sensor.temperature());

        feedback_entry new_data_point;
        new_data_point.board_temperature = board.temperature();
        new_data_point.container_humidity = sensor.humidity();
        new_data_point.container_temperature = sensor.temperature();
        new_data_point.target_temperature = heater.targetTemperature();
        new_data_point.heater_on = heater.isOn();

        queue_add_blocking(&feedback_queue, &new_data_point);
        sleep_ms(DATA_PERIOD_MS);
    }
}

feedback_entry getMostRecentData()
{
    feedback_entry data;
    queue_remove_blocking(&feedback_queue, &data);
    while (!queue_is_empty(&feedback_queue)) {
        queue_remove_blocking(&feedback_queue, &data);
    }
    return data;
}

void initialize()
{
    stdio_init_all();
    adc_init();

    queue_init(&feedback_queue, sizeof(feedback_entry), QUEUE_SIZE);
    queue_init(&request_queue, sizeof(request_entry), QUEUE_SIZE);

    gpio_init(SYSTEM_LED_PIN);
    gpio_set_dir(SYSTEM_LED_PIN, GPIO_OUT);
    gpio_put(SYSTEM_LED_PIN, ON);
}

int main(int argc, char** argv)
{
    initialize();

    uint64_t board_id = systemIdentifier();
    uint32_t count = 0;
    bool mqtt_initialized = false;

    WifiConnection wifi(SSID, PASSPHRASE);
    mqtt::Client mqtt(MQTT_BROKER, CONFIGURED_MQTT_PORT, DEVICE_NAME, MQTT_FEEDBACK_PIN);

    multicore_launch_core1(controlLoop);

    while (true) {
        if (!mqtt.connected()) {
            printf("Connecting MQTT...\n");
            mqtt_initialized = false;
            mqtt.connect();
            sleep_ms(MQTT_CONNECTION_WAIT_MS);
            continue;
        }

        if (!mqtt_initialized) {
            printf("Initializing MQTT...\n");
            mqtt_initialized = mqtt::initialize(mqtt, board_id);
            sleep_ms(COMMUNICATION_PERIOD_MS);
            continue;
        }

        feedback_entry data = getMostRecentData();

        printf("\n----------------- [%u]\n", count);

        wifi.poll();

        printf("Temperature: %.1fC, Humidity: %.1f%%\n", data.container_temperature, data.container_humidity);
        printf("Wifi Connection Status: %s (%s)\n", toString(wifi.status()).data(), wifi.ipAddress().c_str());
        printf("CPU Temperature: %.1fC\n", data.board_temperature);
        printf("MQTT Status: %s\n", mqtt.connected() ? "true" : "false");

        printf("-----------------\n");

        sleep_ms(COMMUNICATION_PERIOD_MS);
        count++;
    }

    return 0;
}
