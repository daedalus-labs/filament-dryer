/*------------------------------------------------------------------------------
Copyright 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include <cstddef>
#include <cstdint>


namespace sensors {
/**
 * Internal sensors provided by the Pico Hardware.
 */
class Board
{
public:
    /** Constructor. */
    Board();

    /**
     * @return The board temperature in degrees Celsius.
     */
    float temperature() const;
};
} // namespace sensors
