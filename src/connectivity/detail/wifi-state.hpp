/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include <lwip/pbuf.h>
#include <lwip/tcp.h>

#include <cstdint>


inline constexpr uint32_t BUFFER_SIZE = 2048;

struct WifiState
{
    struct tcp_pcb* control;
    ip_addr_t remote_address;
    uint16_t remote_port;
    uint8_t buffer[BUFFER_SIZE];
    int32_t buffer_length;
    int32_t sent_length;
    int32_t run_count;
    ConnectionStatus status;
    bool complete;
    bool connected;
};
