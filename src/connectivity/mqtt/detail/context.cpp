/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/

#include "connectivity/mqtt/detail/context.hpp"

#include <cstdint>
#include <cstdio>
#include <functional>
#include <string>
#include <unordered_map>


namespace mqtt::detail {
void ContextInterface::setConnectionStatusCallback(ConnectionStatusCallback callback)
{
    _connection_callback = std::move(callback);
}

void ContextInterface::onConnectionStatusChanged(mqtt_connection_status_t status)
{
    if (_connection_callback) {
        _connection_callback(status);
    }
}

void ContextInterface::subscribe(std::string_view topic, TopicCallback callback)
{
    std::string topic_key(topic.data(), topic.size());
    _callbacks[topic_key] = std::move(callback);
}

void ContextInterface::unsubscribe(std::string_view topic)
{
    std::string topic_key(topic.data(), topic.size());
    auto callback_iteration = _callbacks.find(topic_key);
    if (callback_iteration != _callbacks.cend()) {
        _callbacks.erase(callback_iteration);
    }
}

void ContextInterface::addPendingData(const uint8_t* data, uint16_t length)
{
    size_t desired = _current_index + length;
    printf("Context receiving %u bytes on %s\n", length, _pending_topic.c_str());
    if (desired > _buffer.size()) {
        printf("Context buffer overrun (%u available, %u provided)", _buffer.size(), desired);
        return;
    }

    for (uint16_t i = 0; i < length; _current_index++, i++, _remaining_data--) {
        _buffer[_current_index] = data[i];
    }

    if (_remaining_data <= 0) {
        printf("Context received all data on %s\n", _pending_topic.c_str());
        _push();
    }
}

void ContextInterface::setPendingTopic(const std::string& pending_topic, uint32_t pending_data)
{
    printf("Context expecting %s (length: %u)\n", pending_topic.c_str(), pending_data);

    _pending_topic = pending_topic;
    _current_index = 0;
    _remaining_data = pending_data;
    _buffer.clear();
    if (pending_data == 0) {
        _push();
    }

    _buffer.resize(pending_data);
}

ContextInterface::ContextInterface()
    : _pending_topic(), _buffer(), _current_index(0), _remaining_data(0), _callbacks(), _connection_callback()
{}

void ContextInterface::_push()
{
    auto callback_iteration = _callbacks.find(_pending_topic);
    if (callback_iteration == _callbacks.cend() || !(callback_iteration->second)) {
        printf("No callback registered for %s\n", _pending_topic.c_str());
        return;
    }

    printf("Executing callback for %s\n", _pending_topic.c_str());
    callback_iteration->second(_pending_topic, _buffer);
}

Context& Context::instance()
{
    static Context context;
    return context;
}

ContextInterface& Context::interface()
{
    return _interface;
}

Context::Context() : _interface()
{}
} // namespace mqtt::detail
