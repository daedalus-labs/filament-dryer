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
    char mqtt_topic[TOPIC_BUFFER_SIZE];
    snprintf(mqtt_topic, TOPIC_BUFFER_SIZE, VERSION_TOPIC_FORMAT.data(), client.deviceName().c_str());
    if (!client.publish(mqtt_topic, static_cast<const void*>(VERSION.data()), VERSION.size(), mqtt::QoS::EXACTLY_ONCE, true)) {
        printf("Failed to publish %s\n", mqtt_topic);
        return false;
    }

    snprintf(mqtt_topic, TOPIC_BUFFER_SIZE, UID_TOPIC_FORMAT.data(), client.deviceName().c_str());
    if (!client.publish(mqtt_topic, static_cast<const void*>(uid.c_str()), uid.size(), mqtt::QoS::EXACTLY_ONCE, true)) {
        printf("Failed to publish %s\n", mqtt_topic);
        return false;
    }

    return true;
}

static bool publish(Client& client, const char* topic, const std::string& data)
{
    if (!client.publish(topic, static_cast<const void*>(data.c_str()), data.size(), mqtt::QoS::EXACTLY_ONCE, true)) {
        printf("Failed to publish %s on %s\n", data.c_str(), topic);
        return false;
    }

    return true;
}

static bool publish(Client& client, const char* topic, uint32_t data)
{
    if (!client.publish(topic, static_cast<const void*>(&data), sizeof(uint32_t), mqtt::QoS::EXACTLY_ONCE, true)) {
        printf("Failed to publish %u on %s\n", data, topic);
        return false;
    }

    return true;
}
} // namespace mqtt
