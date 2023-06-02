/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/

#include "utilities.hpp"

#include "gpio.hpp"

#include <hardware/flash.h>
#include <hardware/gpio.h>
#include <pico/time.h>
#include <pico/types.h>
#include <pico/unique_id.h>

#include <cstdint>
#include <cstring>


inline constexpr size_t UID_SIZE = 2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1;
inline constexpr size_t CONFIGURATION_OFFSET = PICO_FLASH_SIZE_BYTES - sizeof(uint32_t);
inline constexpr size_t CONFIGURATION_MAX_SIZE = 1024;
inline constexpr size_t STRING_MAX_SIZE = 256;
inline constexpr int32_t STRING_IS_TOO_BIG = -1;


static int32_t consumeString(const std::vector<uint8_t>& serialized_data, size_t starting_index, std::string& deserialized_data)
{
    uint32_t string_size;
    std::memcpy(&string_size, &serialized_data[starting_index], sizeof(uint32_t));
    size_t string_index = starting_index + sizeof(uint32_t);
    size_t end_index = string_index + string_size;

    printf("Reading configuration string from index %lu\n", starting_index);
    if (string_size > STRING_MAX_SIZE || end_index > serialized_data.size()) {
        printf("Failed to read: [String size: %u, End: %u, data size: %lu]\n", string_size, end_index, serialized_data.size());
        return STRING_IS_TOO_BIG;
    }

    auto string_start = serialized_data.begin() + string_index;
    auto string_end = serialized_data.begin() + end_index;
    deserialized_data.assign(string_start, string_end);
    // This cast can be done naively because of the check against STRING_MAX_SIZE
    return static_cast<int32_t>(string_size + sizeof(uint32_t));
}

SystemConfiguration::SystemConfiguration() : _ssid(), _passphrase(), _mqtt_broker(), _device_name()
{}

SystemConfiguration::SystemConfiguration(const std::vector<uint8_t>& configuration) : _ssid(), _passphrase(), _mqtt_broker(), _device_name()
{
    _parse(configuration);
}

const std::string& SystemConfiguration::deviceName() const
{
    return _device_name;
}

const std::string& SystemConfiguration::mqttBroker() const
{
    return _mqtt_broker;
}

const std::string& SystemConfiguration::passphrase() const
{
    return _passphrase;
}

const std::string& SystemConfiguration::ssid() const
{
    return _ssid;
}

void SystemConfiguration::_parse(const std::vector<uint8_t>& configuration)
{
    // The order here is paramount.
    // The configuration file is (in forward order):
    //   - SSID Length (Fixed 4-byte length)
    //   - SSID
    //   - Passphrase Length (Fixed 4-byte length)
    //   - Passphrase
    //   - MQTT Broker Length (Fixed 4-byte length)
    //   - MQTT Broker
    //   - Device Name Length (Fixed 4-byte length)
    //   - Device Name
    size_t current_index = 0;
    int32_t consumed_bytes = consumeString(configuration, current_index, _ssid);
    current_index += consumed_bytes;
    if (consumed_bytes < 0 || current_index >= configuration.size()) {
        printf("Failed to read SSID from configuration: %d\n", consumed_bytes);
        return;
    }

    consumed_bytes = consumeString(configuration, current_index, _passphrase);
    current_index += consumed_bytes;
    if (consumed_bytes < 0 || current_index >= configuration.size()) {
        printf("Failed to read Passphrase from configuration: %d\n", consumed_bytes);
    }

    consumed_bytes = consumeString(configuration, current_index, _mqtt_broker);
    current_index += consumed_bytes;
    if (consumed_bytes < 0 || current_index >= configuration.size()) {
        printf("Failed to read MQTT Broker from configuration: %d\n", consumed_bytes);
    }

    consumed_bytes = consumeString(configuration, current_index, _device_name);
    if (consumed_bytes < 0) {
        printf("Failed to read Device Name from configuration: %d\n", consumed_bytes);
    }
}

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

bool read(SystemConfiguration& cfg)
{
    // The order here is paramount.
    // The configuration file is (in forward order):
    //   - <Configuration Data>
    //   - Total Configuration Length (Fixed 4-byte length)
    //
    // Meaning the last 4 bytes needs to be read off and some pointer math needs to be done to
    // grab the configuration data.
    //
    // It should be noted that CONFIGURATION_OFFSET already shifts "up" 4-bytes to account for the length.
    uint8_t* configuration_length_location = (uint8_t*)(XIP_BASE + CONFIGURATION_OFFSET);
    uint32_t total_length = *((uint32_t*)configuration_length_location);
    uint8_t* configuration_contents = (uint8_t*)(configuration_length_location - total_length);

    // Rather than consuming in place, the entire chunk of configuration data will be read into
    // a vector and then passed to the configuration object. Although not very efficient, as memory
    // is duplicated twice, this does mean the SystemConfiguration class can be responsible for
    // understanding the configuration contents, and a lot of pointer math is avoided. As of time of
    // implementation, the configuration data is on the order of 50-60 bytes, so this appears like
    // a small hit in memory efficiency for a more object-oriented design.
    printf("Configuration contents are %u bytes @ 0x%08X\n", total_length, configuration_contents);
    std::vector<uint8_t> configuration(total_length);
    for (size_t i = 0; i < total_length; i++, configuration_contents++) {
        configuration[i] = *(configuration_contents);
    }
    cfg = SystemConfiguration(configuration);
    return true;
}
