/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once


#include <pico/time.h>
#include <pico/types.h>

#include <cstdint>
#include <string>
#include <vector>


inline constexpr float C_TO_F_SCALE = 9.0 / 5.0;
inline constexpr float C_TO_F_OFFSET = 32.0;

/**
 * Class encapsulating the configuration data for a device.
 */
class SystemConfiguration
{
public:
    /** Constructor */
    SystemConfiguration();

    /**
     * Constructor
     *
     * @param[in] configuration The configuration data read from flash memory.
     */
    SystemConfiguration(const std::vector<uint8_t>& configuration);

    /**
     * @return The device name to be used for MQTT communication.
     */
    const std::string& deviceName() const;

    /**
     * @note The MQTT Broker can be an IPv4 address or a hostname. This should be passed through a DNS resolver.
     * @return The MQTT Broker.
     */
    const std::string& mqttBroker() const;

    /**
     * @return The passphrase for the configured wireless network.
     */
    const std::string& passphrase() const;

    /**
     * @return The SSID of the configured wireless network.
     */
    const std::string& ssid() const;

private:
    /**
     * Helper function to parse @a configuration.
     *
     * @param[in] configuration The configuration data read from flash memory.
     */
    void _parse(const std::vector<uint8_t>& configuration);

    std::string _ssid;
    std::string _passphrase;
    std::string _mqtt_broker;
    std::string _device_name;
};

/**
 * @return The microseconds since boot.
 */
uint64_t microseconds();

/**
 * @return The milliseconds since boot.
 */
uint64_t milliseconds();

/**
 * @return The unique identifier of this system.
 */
std::string systemIdentifier();

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
 * Reads the system configuration information from flash memory.
 *
 * @param[in] cfg The system configuration information to be read.
 * @return True if the read was successful, false otherwise.
 */
bool read(SystemConfiguration& cfg);

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
