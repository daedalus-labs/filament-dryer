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
#include <pico/unique_id.h>
#include <pico/util/queue.h>

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <functional>


inline constexpr float HEATER_HYSTERESIS = 2.5f;
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

static void publish(mqtt::Client& client, const feedback_entry& data)
{
    std::string board_temperature = std::to_string(data.board_temperature);
    std::string container_temperature = std::to_string(data.container_temperature);
    std::string container_humidity = std::to_string(data.container_humidity);
    std::string target_temperature = std::to_string(data.target_temperature);

    char mqtt_topic[TOPIC_BUFFER_SIZE];
    snprintf(mqtt_topic, TOPIC_BUFFER_SIZE, BOARD_TEMPERATURE_TOPIC_FORMAT.data(), client.deviceName().c_str());
    mqtt::publish(client, mqtt_topic, board_temperature);

    snprintf(mqtt_topic, TOPIC_BUFFER_SIZE, HUMIDITY_TOPIC_FORMAT.data(), client.deviceName().c_str());
    mqtt::publish(client, mqtt_topic, container_humidity);

    snprintf(mqtt_topic, TOPIC_BUFFER_SIZE, TEMPERATURE_TOPIC_FORMAT.data(), client.deviceName().c_str());
    mqtt::publish(client, mqtt_topic, container_temperature);

    snprintf(mqtt_topic, TOPIC_BUFFER_SIZE, HEATER_TOPIC_FORMAT.data(), client.deviceName().c_str());
    mqtt::publish(client, mqtt_topic, static_cast<uint32_t>(data.heater_on));

    snprintf(mqtt_topic, TOPIC_BUFFER_SIZE, TARGET_TEMPERATURE_TOPIC_FORMAT.data(), client.deviceName().c_str());
    mqtt::publish(client, mqtt_topic, target_temperature);
}

static void initialize()
{
    stdio_init_all();
    adc_init();

    queue_init(&feedback_queue, sizeof(feedback_entry), QUEUE_SIZE);
    queue_init(&request_queue, sizeof(request_entry), QUEUE_SIZE);

    gpio_init(SYSTEM_LED_PIN);
    gpio_set_dir(SYSTEM_LED_PIN, GPIO_OUT);
    gpio_put(SYSTEM_LED_PIN, ON);
}

static void onSetTargetTemperatureReceived(const std::string& topic, const mqtt::Buffer& data)
{
    errno = 0;
    request_entry set_request;
    std::string value = mqtt::toString(data);
    set_request.target_temperature = strtof(value.c_str(), NULL);

    if (errno) {
        printf("Failed to handle %s: %u\n", topic.c_str(), errno);
        return;
    }

    printf("Received request to set target temperature to %.1fC\n", set_request.target_temperature);
    queue_add_blocking(&request_queue, &set_request);
}

static bool initialize_mqtt(mqtt::Client& client, const std::string& board_id)
{
    if (!mqtt::initialize(client, board_id)) {
        printf("Failed to initialize MQTT\n");
        return false;
    }

    char mqtt_topic[TOPIC_BUFFER_SIZE];
    snprintf(mqtt_topic, TOPIC_BUFFER_SIZE, SET_TARGET_TEMPERATURE_TOPIC_FORMAT.data(), client.deviceName().c_str());
    if (!client.subscribe(mqtt_topic, onSetTargetTemperatureReceived)) {
        printf("Failed to subscribe to %s\n", mqtt_topic);
        return false;
    }

    printf("Successfully initialized MQTT\n");
    return true;
}

int main(int argc, char** argv)
{
    initialize();
    std::string board_id = systemIdentifier();
    uint32_t count = 0;
    bool mqtt_initialized = false;

    sleep_ms(5000);
    SystemConfiguration cfg;
    if (!read(cfg)) {
        printf("Failed to read system configuration\n");
        return EXIT_FAILURE;
    }

    WifiConnection wifi(cfg.ssid(), cfg.passphrase());
    mqtt::Client mqtt(cfg.mqttBroker(), CONFIGURED_MQTT_PORT, cfg.deviceName(), MQTT_FEEDBACK_PIN);

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
            mqtt_initialized = initialize_mqtt(mqtt, board_id);
            sleep_ms(COMMUNICATION_PERIOD_MS);
            continue;
        }

        feedback_entry data = getMostRecentData();
        publish(mqtt, data);

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

    return EXIT_SUCCESS;
}
