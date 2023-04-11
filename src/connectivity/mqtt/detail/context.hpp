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
class ContextInterface
{
public:
    ContextInterface();

    void setConnectionStatusCallback(ConnectionStatusCallback callback);

    void onConnectionStatusChanged(mqtt_connection_status_t status);

    void subscribe(std::string_view topic, TopicCallback callback);

    void unsubscribe(std::string_view topic);

    void addPendingData(const uint8_t* data, uint16_t length);

    void setPendingTopic(const std::string& pending_topic, uint32_t pending_data);

private:
    void _push();

    std::string _pending_topic;
    Buffer _buffer;
    size_t _current_index;
    ssize_t _remaining_data;
    std::unordered_map<std::string, TopicCallback> _callbacks;
    ConnectionStatusCallback _connection_callback;
};

class Context
{
public:
    static Context& instance();

    ContextInterface& interface();

private:
    Context();

    ContextInterface _interface;
};

inline ContextInterface& context()
{
    return Context::instance().interface();
}
} // namespace mqtt::detail
