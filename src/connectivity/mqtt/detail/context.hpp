/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include "connectivity/mqtt/common.hpp"

#include <cstdint>
#include <cstdio>
#include <functional>
#include <string>
#include <unordered_map>


namespace mqtt::detail {
/**
 * An interface for modifying the MQTT context.
 */
class ContextInterface
{
public:
    /** Constructor. */
    ContextInterface();

    /**
     * Sets the callback for connectivity status changes.
     *
     * @param[in] callback The callback to use when MQTT connectivity changes.
     */
    void setConnectionStatusCallback(ConnectionStatusCallback callback);

    /**
     * Handler for changes to the MQTT connectivity status.
     *
     * @note This should not be called by MQTT clients.
     * @param[in] status The MQTT connection status.
     */
    void onConnectionStatusChanged(mqtt_connection_status_t status);

    /**
     * Subscribes to a MQTT topic.
     *
     * @param[in] topic The topic to be subscribed to.
     * @param[in] callback The callback to be invoked when a complete message has been received on @a topic.
     */
    void subscribe(std::string_view topic, TopicCallback callback);

    /**
     * Unsubscribes from a MQTT topic.
     *
     * @param[in] topic The topic to be unsubscribed from.
     */
    void unsubscribe(std::string_view topic);

    /**
     * Adds data to the pending MQTT topic.
     *
     * @param[in] data The buffer of data to be added.
     * @param[in] length The number of bytes in the data buffer.
     */
    void addPendingData(const uint8_t* data, uint16_t length);

    /**
     * Sets the topic on which the MQTT stack is about to receive data.
     *
     * @param[in] pending_topic The topic on which data is about to be received.
     * @param[in] pending_data The amount of data to be received.
     */
    void setPendingTopic(const std::string& pending_topic, uint32_t pending_data);

private:
    /**
     * Pushes pending MQTT data to any subscribed clients.
     */
    void _push();

    std::string _pending_topic;
    Buffer _buffer;
    size_t _current_index;
    ssize_t _remaining_data;
    std::unordered_map<std::string, TopicCallback> _callbacks;
    ConnectionStatusCallback _connection_callback;
};

/**
 * The MQTT context, responsible for routing feedback from the MQTT stack to subscribed classes.
 *
 * @note This class is a singleton.
 */
class Context
{
public:
    /**
     * @return The MQTT context.
     */
    static Context& instance();

    /**
     * @return The interface for modifying the MQTT context.
     */
    ContextInterface& interface();

private:
    /** Constructor. */
    Context();

    ContextInterface _interface;
};

/**
 * @return The MQTT context.
 */
inline ContextInterface& context()
{
    return Context::instance().interface();
}
} // namespace mqtt::detail
