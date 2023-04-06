/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>


/**
 * Enumerates the types of DHT sensors supported.
 */
enum class DHTType : uint8_t
{
    /** DHT 11 type sensor. */
    DHT11,

    /** DHT 21 type sensor. */
    DHT21,

    /** DHT 22 type sensor. */
    DHT22
};

/**
 * An implementation of the DHT sensor compatible with the Raspberry Pi Pico.
 *
 * @note See https://www.waveshare.com/wiki/DHT22_Temperature-Humidity_Sensor
 */
class DHT
{
public:
    static constexpr size_t FRAME_SIZE = 5;

    using Frame = std::array<uint8_t, FRAME_SIZE>;

    /**
     * Constructor.
     *
     * @param[in] type The type of sensor.
     * @param[in] data_pin The data pin of the DHT sensor.
     * @param[in] feedback_led_pin The pin of the LED to toggle on/off. If provided, the LED will be on when data is being read from the
     * sensor, and off otherwise.
     */
    DHT(DHTType type, uint8_t data_pin, uint8_t feedback_led_pin);

    /**
     * @return The measured temperature in degrees Celsius.
     */
    float temperature() const;

    /**
     * @return The measured humidity as a percentage.
     */
    float humidity() const;

    /**
     * @return The type of DHT sensor.
     */
    DHTType type() const;

    void read();

private:
    /**
     * Checks for the sensor's response indicating it is ready to be read from.
     *
     * @return true if the sensor is ready, false otherwise.
     */
    bool _checkResponse() const;

    /**
     * Reads a bit from the sensor's data pin according to the protocol used by DHT sensors.
     *
     * @return true if the bit is high, false if the bit is low.
     */
    bool _getDataBit() const;

    /**
     * Reads a byte from the sensor's data pin according to the protocol used by DHT sensors.
     *
     * @return The byte read from the sensor.
     */
    uint8_t _getDataByte() const;

    /**
     * Parses the provided frame into a humidity and temperate.
     *
     * @param[in] data The data frame read from the sensor
     */
    void _parse(const Frame& data);

    /**
     * Reads data from the sensor.
     */
    void _read();

    /**
     * Sets the LED to the value indicated by @a state.
     *
     * @param[in] state The desired state of the LED.
     */
    void _setLED(uint8_t state) const;

    /**
     * Starts the process of reading a data frame from the sensor.
     */
    void _start();

    float _humidity;
    float _temperature;
    DHTType _type;
    uint8_t _data_pin;
    uint8_t _feedback_led_pin;
};
