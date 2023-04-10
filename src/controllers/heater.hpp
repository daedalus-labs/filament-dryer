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
    Heater(uint8_t control_pin, uint8_t feedback_pin, float hysteresis, uint64_t max_on_time);

    bool isOn() const;

    void setActualTemperature(float actual_temperature);
    void setTargetTemperature(float target_temperature);

private:
    void _off();
    void _on();

    const uint64_t _max_on_time;
    uint8_t _feedback_pin;
    uint8_t _control_pin;
    float _hysteresis;
    float _target_temperature;
    uint64_t _on_time;
    uint64_t _off_time;
};
} // namespace controllers
