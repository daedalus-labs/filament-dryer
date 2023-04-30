/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include "connectivity/mqtt/common.hpp"

#include <lwip/apps/mqtt.h>
#include <lwip/ip_addr.h>

#include <cstdint>
#include <string>
#include <string_view>


namespace mqtt {
/**
 * The MQTT QoS values.
 *
 * @see https://www.hivemq.com/blog/mqtt-essentials-part-6-mqtt-quality-of-service-levels/
 */
enum class QoS : uint8_t
{
    /** MQTT will deliver the message at most once. */
    AT_MOST_ONCE = 0,

    /** MQTT will deliver the message at least once. */
    AT_LEAST_ONCE = 1,

    /** MQTT will deliver the message exactly once. */
    EXACTLY_ONCE = 2
};

class Client
{
public:
    /**
     * Constructor
     *
     * @param[in] broker The MQTT Broker.
     * @param[in] port The MQTT port on which @a broker is listening.
     * @param[in] client_name The name of this MQTT client.
     * @param[in] led_pin The MQTT Feedback LED pin to use.
     */
    Client(std::string_view broker, uint16_t port, std::string_view client_name, uint8_t led_pin);

    /**
     * Constructor
     *
     * @param[in] broker The MQTT Broker.
     * @param[in] port The MQTT port on which @a broker is listening.
     * @param[in] client_name The name of this MQTT client.
     * @param[in] user The username of this MQTT client.
     * @param[in] password The password associated with @a user.
     * @param[in] led_pin The MQTT Feedback LED pin to use.
     */
    Client(std::string_view broker,
           uint16_t port,
           std::string_view client_name,
           std::string_view user,
           std::string_view password,
           uint8_t led_pin);

    /** Destructor. */
    ~Client();

    /**
     * @return True if this client connected to MQTT, false otherwise.
     */
    bool connect();

    /**
     * @return True if this client is connected to MQTT, false otherwise.
     */
    bool connected() const;

    /**
     * @return True if this client disconnected from MQTT, false otherwise.
     */
    bool disconnect();

    /**
     * Publish an MQTT message on @a topic.
     *
     * @param[in] topic The MQTT topic on which to publish the message.
     * @param[in] payload The message payload.
     * @param[in] size The size of @a payload in bytes.
     * @param[in] qos The QoS to use for this publish.
     * @param[in] retain True if the broker should retain this message, false otherwise.
     * @return True if the MQTT message was published, false otherwise.
     */
    bool publish(std::string_view topic, const void* payload, uint16_t size, QoS qos, bool retain);

    /**
     * Subscribes to an MQTT topic, @a topic, using this Client's connection.
     *
     * @param[in] topic The MQTT topic to subscribe to.
     * @param[in] callback The callback to be invoked when data is available on @a topic.
     * @return True if @a topic was subscribed to, false otherwise.
     */
    bool subscribe(std::string_view topic, TopicCallback callback);

    /**
     * Unsubscribes from an MQTT topic, @a topic, using this Client's connection.
     *
     * @param[in] topic The MQTT topic to unsubscribe from.
     * @return True if @a topic was unsubscribed from, false otherwise.
     */
    bool unsubscribe(std::string_view topic);

private:
    /**
     * Initializes this MQTT client.
     */
    void _init();

    /**
     * Handler for MQTT connection status changes.
     *
     * @param[in] status
     */
    void _onConnectionStatusChanged(mqtt_connection_status_t status);

    mqtt_client_t* _mqtt;
    uint8_t _led_pin;
    std::string _broker;
    uint16_t _port;
    ip_addr_t _broker_address;
    std::string _name;
    std::string _user;
    std::string _password;
    mqtt_connect_client_info_t _info;
};
} // namespace mqtt
