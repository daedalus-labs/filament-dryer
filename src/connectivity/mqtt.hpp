/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include "connectivity/mqtt/client.hpp"
#include "generated/configuration.hpp"

#include <pico/stdio.h>

#include <cstdio>


namespace mqtt {
static bool initialize(Client& client, uint64_t uid)
{
    if (!client.publish(VERSION_TOPIC, static_cast<const void*>(VERSION.data()), VERSION.size(), mqtt::QoS::EXACTLY_ONCE, true)) {
        printf("Failed to publish %s\n", VERSION_TOPIC.data());
        return false;
    }

    if (!client.publish(UID_TOPIC, static_cast<const void*>(&uid), sizeof(uint64_t), mqtt::QoS::EXACTLY_ONCE, true)) {
        printf("Failed to publish %s\n", UID_TOPIC.data());
        return false;
    }

    return true;
}
} // namespace mqtt
