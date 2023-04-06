/*------------------------------------------------------------------------------
Copyright (c) 2023 Joe Porembski
SPDX-License-Identifier: BSD-3-Clause
------------------------------------------------------------------------------*/

#include "connectivity/mqtt/client.hpp"

#include "connectivity/constants.hpp"
#include "connectivity/dns/resolver.hpp"

#include <lwip/apps/mqtt.h>
#include <pico/cyw43_arch.h>

#include <cstdint>
#include <cstdio>


/**
 * Callback for handling connection results of a connection request.
 *
 * Called when a connection request has completed.
 *
 * @param[in] client The MQTT client handle.
 * @param[in] arg Additional data passed to the callback.
 * @param[in] status The connection status.
 */
static void onConnectionComplete(mqtt_client_t* client, void* arg, mqtt_connection_status_t status)
{
    printf("Connection Status: %d\n", static_cast<int32_t>(status));
}

/**
 * Callback for handling incoming message data.
 *
 * Called when data is received on a subscribed topic.
 *
 * @note @a flags will be MQTT_DATA_FLAG_LAST when all message data has been received.
 * @param[in] arg Additional data passed to the callback.
 * @param[in] data The amount of message data provided to this callback, or NULL if all data has been received.
 * @param[in] length The length of the fragment of message data.
 * @param[in] flags Any MQTT flags set by the caller.
 */
static void onDataReceived(void* arg, const u8_t* data, u16_t length, u8_t flags)
{
    // This function is intentionally empty
    //  - It is being left as a skeleton for future use.
}

/**
 * Callback for handling a publish notification on a subscribed topic.
 *
 * Called when the client is notified of a publish on a subscribed topic.
 *
 * @param[in] arg Additional data passed to the callback.
 * @param[in] topic The topic name.
 * @param[in] total_length The total length of the data published at @a topic.
 */
static void onTopicUpdated(void* arg, const char* topic, u32_t total_length)
{
    // This function is intentionally empty
    //  - It is being left as a skeleton for future use.
}

/**
 * Callback for handling a completed request.
 *
 * Called when a subscribe, unsubscribe or publish request has completed
 *
 * @param[in] arg Additional data passed to the callback.
 * @param[in] error The error which occurred, or ERR_OK if no error occurred.
 */
static void onRequestComplete(void* arg, err_t error)
{
    if (error != ERR_OK) {
        printf("Previous request failed: %s\n", lwip_strerr(error));
    }
}

namespace mqtt {
inline constexpr uint32_t DNS_TIMEOUT_MS = 10000;

Client::Client(std::string_view broker, uint16_t port, std::string_view client_name)
    : _mqtt(mqtt_client_new()), _broker(broker), _port(port), _broker_address(), _name(client_name), _user(), _password(), _info()
{
    _info.client_id = _name.c_str();
    _info.client_pass = NULL;
    _info.client_user = NULL;
    _info.keep_alive = KEEP_ALIVE_TIMEOUT_S;
    _info.will_msg = NULL;
    _info.will_qos = OFF;
    _info.will_retain = OFF;
    _info.will_topic = NULL;
#if LWIP_ALTCP && LWIP_ALTCP_TLS
    _info.tls_config = NULL;
#endif
}

Client::Client(std::string_view broker, uint16_t port, std::string_view client_name, std::string_view user, std::string_view password)
    : _mqtt(mqtt_client_new()),
      _broker(broker),
      _port(port),
      _broker_address(),
      _name(client_name),
      _user(user),
      _password(password),
      _info()
{
    _info.client_id = _name.c_str();
    _info.client_pass = _password.c_str();
    _info.client_user = _user.c_str();
    _info.keep_alive = KEEP_ALIVE_TIMEOUT_S;
    _info.will_msg = NULL;
    _info.will_qos = OFF;
    _info.will_retain = OFF;
    _info.will_topic = NULL;
#if LWIP_ALTCP && LWIP_ALTCP_TLS
    _info.tls_config = NULL;
#endif
}

Client::~Client()
{
    // Can I disconnect twice safely? Can I check if I'm connected?
    cyw43_arch_lwip_begin();
    mqtt_disconnect(_mqtt);
    mqtt_client_free(_mqtt);
    cyw43_arch_lwip_end();
}

bool Client::connected() const
{
    return mqtt_client_is_connected(_mqtt) >= CONNECTED;
}

bool Client::connect()
{
    if (!dns::resolve(_broker, _broker_address, DNS_TIMEOUT_MS)) {
        printf("Failed to resolve %s when connecting to MQTT\n", _broker.c_str());
        return false;
    }

    printf("Connecting to %s (%s) as %s\n", _broker.c_str(), ip4addr_ntoa(&_broker_address), _info.client_id);
    mqtt_set_inpub_callback(_mqtt, onTopicUpdated, onDataReceived, LWIP_CONST_CAST(void*, &_info));

    cyw43_arch_lwip_begin();
    err_t error = mqtt_client_connect(_mqtt, &_broker_address, _port, onConnectionComplete, LWIP_CONST_CAST(void*, &_info), &_info);
    cyw43_arch_lwip_end();

    if (error != ERR_OK) {
        printf("Connection to %s unsuccessful: %s\n", _broker.c_str(), lwip_strerr(error));
        return false;
    }
    return true;
}

bool Client::disconnect()
{
    cyw43_arch_lwip_begin();
    mqtt_disconnect(_mqtt);
    cyw43_arch_lwip_end();
    return true;
}

bool Client::publish(std::string_view topic, const void* payload, uint16_t size, QoS qos, bool retain)
{
    uint8_t qos_value = static_cast<uint8_t>(qos);
    uint8_t retain_value = static_cast<uint8_t>(retain);

    cyw43_arch_lwip_begin();
    err_t error = mqtt_publish(_mqtt, topic.data(), payload, size, qos_value, retain_value, onRequestComplete, NULL);
    cyw43_arch_lwip_end();

    if (error != ERR_OK) {
        printf("Publish of %s unsuccessful: %s\n", topic.data(), lwip_strerr(error));
        return false;
    }
    return true;
}
} // namespace mqtt
