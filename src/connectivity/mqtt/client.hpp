/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include <lwip/apps/mqtt.h>
#include <lwip/ip_addr.h>

#include <cstdint>
#include <string>
#include <string_view>


namespace mqtt {
enum class QoS : uint8_t
{
    AT_MOST_ONCE = 0,
    AT_LEAST_ONCE = 1,
    EXACTLY_ONCE = 2
};

class Client
{
public:
    Client(std::string_view broker, uint16_t port, std::string_view client_name);
    Client(std::string_view broker, uint16_t port, std::string_view client_name, std::string_view user, std::string_view password);
    ~Client();

    bool connect();
    bool connected() const;
    bool disconnect();
    bool publish(std::string_view topic, const void* payload, uint16_t size, QoS qos, bool retain);

private:
    mqtt_client_t* _mqtt;
    std::string _broker;
    uint16_t _port;
    ip_addr_t _broker_address;
    std::string _name;
    std::string _user;
    std::string _password;
    mqtt_connect_client_info_t _info;
};
} // namespace mqtt
