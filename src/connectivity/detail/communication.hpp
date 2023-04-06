/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include <lwip/pbuf.h>
#include <lwip/tcp.h>


static err_t clientConnected(void* arg, struct tcp_pcb* control, err_t error)
{
    return ERR_TIMEOUT;
}

static void clientError(void* arg, err_t error)
{}

static err_t clientPoll(void* arg, struct tcp_pcb* control)
{
    return ERR_TIMEOUT;
}

static err_t clientReceive(void* arg, struct tcp_pcb* control, struct pbuf* msg, err_t error)
{
    return ERR_TIMEOUT;
}

static err_t clientSent(void* arg, struct tcp_pcb* control, u16_t length)
{
    return ERR_TIMEOUT;
}
