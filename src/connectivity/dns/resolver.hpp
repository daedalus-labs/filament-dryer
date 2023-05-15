/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/
#pragma once

#include <lwip/ip_addr.h>

#include <cstdint>
#include <string>


namespace dns {
/**
 * Resolves a FQDN or hostname to an IP address.
 *
 * @param[in] hostname The hostname or FQDN to be resolved.
 * @param[in] resolved_address The resolved IP address.
 * @param[in] timeout_ms The timeout for IP address resolution.
 * @return True if @a hostname was resolved, false otherwise.
 */
bool resolve(const std::string& hostname, ip_addr_t& resolved_address, int32_t timeout_ms);
} // namespace dns
