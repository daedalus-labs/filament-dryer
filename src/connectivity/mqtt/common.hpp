/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include <lwip/apps/mqtt.h>

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace mqtt {
using Buffer = std::vector<uint8_t>;
using TopicCallback = std::function<void(const std::string&, const Buffer&)>;
using ConnectionStatusCallback = std::function<void(mqtt_connection_status_t)>;

static std::string toString(const Buffer& data)
{
    std::string result;
    result.resize(data.size());
    for (size_t i = 0; i < data.size(); i++) {
        result[i] = static_cast<char>(data[i]);
    }
    return result;
}
} // namespace mqtt