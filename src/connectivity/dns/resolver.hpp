/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include <lwip/ip_addr.h>

#include <cstdint>
#include <string>


namespace dns {
bool resolve(const std::string& hostname, ip_addr_t& resolved_address, int32_t timeout_ms);
} // namespace dns
