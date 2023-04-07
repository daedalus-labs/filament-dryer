/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include <cstdint>
#include <cstdio>
#include <functional>
#include <string>
#include <unordered_map>


namespace mqtt::detail {
class Context
{
public:
    static Context& context()
    {
        static Context instance;
        return instance;
    }

    void setPendingTopic(const std::string& pending_topic, uint32_t pending_data)
    {
        printf("Context expecting %s (length: %u)\n", pending_topic.c_str(), pending_data);

        _pending_topic = pending_topic;
        _current_index = 0;
        _buffer.clear();
        if (pending_data == 0) {
            _push();
        }

        _buffer.resize(pending_data);
    }

    void addPendingData(const uint8_t* data, uint16_t length)
    {
        size_t desired = _current_index + length;
        printf("Context receiving %u bytes on %s\n", length, _pending_topic.c_str());
        if (desired > _buffer.size()) {
            printf("Context buffer overrun (%u available, %u provided)", _buffer.size(), desired);
            return;
        }

        for (uint16_t i = 0; i < length; _current_index++, i++) {
            _buffer[_current_index] = data[i];
        }
    }

private:
    Context();

    void _push();

    std::string _pending_topic;
    std::vector<uint8_t> _buffer;
    size_t _current_index;
};
} // namespace mqtt::detail
