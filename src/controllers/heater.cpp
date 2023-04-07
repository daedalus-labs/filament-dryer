/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#include "controllers/heater.hpp"

#include "constants.hpp"

#include <hardware/gpio.h>
#include <pico/stdio.h>

#include <cstdint>
#include <cstdio>


namespace controllers {
Heater::Heater(uint8_t control_pin, uint8_t feedback_pin, float hysteresis, uint32_t max_on_time)
    : _feedback_pin(feedback_pin),
      _control_pin(control_pin),
      _hysteresis(hysteresis),
      _max_on_time(max_on_time),
      _target_temperature(DEFAULT_TEMPERATURE),
      _on_time()
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

void Heater::setTargetTemperature(float target_temperature)
{
    if (target_temperature < MINIMUM_TARGET_TEMPERATURE) {
        printf("Target temperature of %.1fC must be greater than %.1fC\n", target_temperature, MINIMUM_TARGET_TEMPERATURE);
        return;
    }
    _target_temperature = target_temperature;
}
} // namespace controllers