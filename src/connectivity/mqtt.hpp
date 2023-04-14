/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include "connectivity/mqtt/client.hpp"
#include "connectivity/mqtt/common.hpp"
#include "generated/configuration.hpp"

#include <pico/stdio.h>

#include <cstdio>


namespace mqtt {
static bool initialize(Client& client, const std::string& uid)
{
    if (!client.publish(VERSION_TOPIC, static_cast<const void*>(VERSION.data()), VERSION.size(), mqtt::QoS::EXACTLY_ONCE, true)) {
        printf("Failed to publish %s\n", VERSION_TOPIC.data());
        return false;
    }

    if (!client.publish(UID_TOPIC, static_cast<const void*>(uid.c_str()), uid.size(), mqtt::QoS::EXACTLY_ONCE, true)) {
        printf("Failed to publish %s\n", UID_TOPIC.data());
        return false;
    }

    return true;
}

static bool publish(Client& client, std::string_view topic, const std::string& data)
{
    if (!client.publish(topic, static_cast<const void*>(data.c_str()), data.size(), mqtt::QoS::EXACTLY_ONCE, true)) {
        printf("Failed to publish %s on %s\n", data.c_str(), topic.data());
        return false;
    }

    return true;
}

static bool publish(Client& client, std::string_view topic, uint32_t data)
{
    if (!client.publish(topic, static_cast<const void*>(&data), sizeof(uint32_t), mqtt::QoS::EXACTLY_ONCE, true)) {
        printf("Failed to publish %u on %s\n", data, topic.data());
        return false;
    }

    return true;
}
} // namespace mqtt
