/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include <cstdint>


namespace controllers {
class Heater
{
public:
    Heater(uint8_t control_pin, uint8_t feedback_pin, float hysteresis, uint32_t max_on_time);

    bool isOn() const;

    void setTargetTemperature(float target_temperature);

private:
    uint8_t _feedback_pin;
    uint8_t _control_pin;
    float _hysteresis;
    uint32_t _max_on_time;
    float _target_temperature;
    uint32_t _on_time;
};
} // namespace controllers
