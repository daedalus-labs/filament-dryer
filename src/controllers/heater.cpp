/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#include "controllers/heater.hpp"

#include "constants.hpp"
#include "utilities.hpp"

#include <hardware/gpio.h>
#include <pico/stdio.h>

#include <cstdint>
#include <cstdio>


namespace controllers {
Heater::Heater(uint8_t control_pin, uint8_t feedback_pin, float hysteresis, uint64_t max_on_time)
    : _max_on_time(max_on_time),
      _feedback_pin(feedback_pin),
      _control_pin(control_pin),
      _hysteresis(hysteresis),
      _target_temperature(DEFAULT_TEMPERATURE),
      _on_timepoint(),
      _off_timepoint()
{
    // Hysteresis must be positive in order to prevent the controller from latching into one state.
    if (_hysteresis < MINIMUM_HYSTERESIS) {
        _hysteresis = MINIMUM_HYSTERESIS;
    }

    gpio_init(_control_pin);
    gpio_set_dir(_control_pin, GPIO_OUT);
}

bool Heater::isOn() const
{
    return gpio_get(_control_pin);
}

float Heater::targetTemperature() const
{
    return _target_temperature;
}

void Heater::setTargetTemperature(float target_temperature)
{
    if (target_temperature < MINIMUM_TARGET_TEMPERATURE) {
        printf("Target temperature of %.1fC must be greater than %.1fC\n", target_temperature, MINIMUM_TARGET_TEMPERATURE);
        return;
    }

    if (_target_temperature == target_temperature) {
        printf("Heater already has target temperature of %.1fC\n", target_temperature);
        return;
    }

    printf("Heater has new target temperature of %.1fC\n", target_temperature);
    _target_temperature = target_temperature;
}

void Heater::update(float actual_temperature)
{
    float off_threshold = _target_temperature + _hysteresis;
    float on_threshold = _target_temperature - _hysteresis;
    bool is_on = isOn();
    uint64_t current_timepoint = milliseconds();

    if (is_on && ((actual_temperature > off_threshold) || (current_timepoint - _on_timepoint > _max_on_time))) {
        _off();
    }
    else if (!is_on && (actual_temperature < on_threshold)) {
        _on();
    }
}

void Heater::_off()
{
    _off_timepoint = milliseconds();

    printf("Turning off Heater...\n");
    gpio_put(_control_pin, OFF);
    gpio_put(_feedback_pin, OFF);
}

void Heater::_on()
{
    uint64_t current_timepoint = milliseconds();
    if (current_timepoint - _off_timepoint < MINIMUM_OFF_TIME_MS) {
        printf("Cannot enable heater, has not been off for %u milliseconds\n", MINIMUM_OFF_TIME_MS);
        return;
    }

    printf("Turning on Heater...\n");
    _on_timepoint = current_timepoint;
    gpio_put(_control_pin, ON);
    gpio_put(_feedback_pin, ON);
}
} // namespace controllers