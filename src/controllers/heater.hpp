/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include <cstdint>


namespace controllers {
/**
 * A generic controller for a Heating element.
 */
class Heater
{
public:
    /**
     * Constructor.
     *
     * @param[in] control_pin The pin assigned to the Heater Relay.
     * @param[in] feedback_pin The pin assigned to the Heater Feedback LED.
     * @param[in] hysteresis The hysteresis in degrees Celsius.
     * @param[in] max_on_time The maximum amount of time the heater can be on in milliseconds.
     */
    Heater(uint8_t control_pin, uint8_t feedback_pin, float hysteresis, uint64_t max_on_time);

    /**
     * @return True if the heater is on, false otherwise.
     */
    bool isOn() const;

    /**
     * @return The current target temperature in degrees Celsius.
     */
    float targetTemperature() const;

    /**
     * Sets the target temperature for the heater.
     *
     * @param[in] target_temperature The desired target temperature in degrees Celsius.
     */
    void setTargetTemperature(float target_temperature);

    /**
     * Updates the Heater's control based on the provided temperature.
     *
     * @param[in] actual_temperature The actual temperature of the environment in degrees Celsius.
     */
    void update(float actual_temperature);

private:
    /**
     * Turn the Heater off.
     */
    void _off();

    /**
     * Turn the Heater on.
     */
    void _on();

    const uint64_t _max_on_time;
    uint8_t _feedback_pin;
    uint8_t _control_pin;
    float _hysteresis;
    float _target_temperature;
    uint64_t _on_timepoint;
    uint64_t _off_timepoint;
};
} // namespace controllers
