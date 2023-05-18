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

static int32_t consumeString(const uint8_t* memory_contents, std::string& data)
{
    data.clear();
    int32_t bytes_consumed = 0;

    // A string in memory is encoded as: ASCII string contents followed by a 4-byte string length
    // The first step is to shift the pointer back by 4-bytes.
    const uint8_t* length_location = memory_contents - sizeof(uint32_t);
    uint32_t string_length = *((uint32_t*)length_location);
    bytes_consumed += sizeof(uint32_t);
    if (string_length > STRING_MAX_SIZE) {
        printf("Invalid string length (%lu)\n", string_length);
        return -1;
    }

    if (string_length == 0) {
        return bytes_consumed;
    }

    // The read process will need to "rewind" from the current position
    // to the head of the string, which is string_length bytes ahead
    const uint8_t* string_location = memory_contents + (string_length + sizeof(uint32_t));
    data.assign((const char*)(string_location), string_length);
    bytes_consumed += string_length;
    return bytes_consumed;
}


SystemConfiguration::SystemConfiguration() : _ssid(), _passphrase()
{}

const std::string& SystemConfiguration::passphrase() const
{
    return _passphrase;
}

const std::string& SystemConfiguration::ssid() const
{
    return _ssid;
}

void SystemConfiguration::setPassphrase(const std::string& value)
{
    _passphrase = value;
}

void SystemConfiguration::setSSID(const std::string& value)
{
    _ssid = value;
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
    //  - SSID
    //  - SSID Length (4-byte width)
    //  - Passphrase
    //  - Passphrase Length (4-byte width)
    //  - Total Length (4-byte width)
    //
    // This code will read the data backwards, so the values must be read in reverse order.

    uint8_t* configuration_contents = (uint8_t*)(XIP_BASE + CONFIGURATION_OFFSET);
    uint32_t total_length = *((uint32_t*)configuration_contents);

    if (total_length > CONFIGURATION_MAX_SIZE) {
        printf("Invalid configuration length (%lu)\n", total_length);
        return false;
    }

    if (total_length == 0) {
        printf("No configuration file written\n");
        return true;
    }

    // Need to account for the total configuration length consumed
    configuration_contents -= sizeof(uint32_t);
    std::string ssid, passphrase;
    int32_t bytes_consumed = consumeString(configuration_contents, passphrase);
    if (bytes_consumed < 0) {
        printf("Failed to read passphrase from configuration\n");
        return false;
    }

    configuration_contents -= bytes_consumed;
    if (consumeString(configuration_contents, ssid) < 0) {
        printf("Failed to read SSID from configuration\n");
        return false;
    }

    cfg.setSSID(ssid);
    cfg.setPassphrase(passphrase);
    return true;
}
