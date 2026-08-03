//
// Created by Wiktor on 24.07.2026.
//

#include "../include/MqttTransport.h"

#include <esp_log.h>

#include "mqtt_client.h"

static constexpr uint8_t MQTT_DISCONNECTED_BIT = BIT0;
static constexpr uint8_t MQTT_PUBLISHED_BIT = BIT1;
static constexpr uint8_t MQTT_ERROR_BIT = BIT2;


esp_err_t MqttTransport::connect() {
    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.hostname = this->m_brokerIp;
    mqtt_cfg.broker.address.port = this->m_brokerPort;
    mqtt_cfg.broker.address.transport = MQTT_TRANSPORT_OVER_TCP;
    mqtt_cfg.session.protocol_ver = MQTT_PROTOCOL_V_5;
    mqtt_cfg.session.disable_clean_session = false;

    esp_mqtt5_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    if (client == nullptr) {
        return ESP_FAIL;
    }
    this->m_client = client;

    esp_err_t result = ESP_OK;
    result = esp_mqtt_client_register_event(client, static_cast<esp_mqtt_event_id_t>(ESP_EVENT_ANY_ID), mqtt5_event_handler, this);
    if (result != ESP_OK) {
        return result;
    }
    result = esp_mqtt_client_start(client);
    if (result != ESP_OK) {
        return result;
    }

    EventBits_t bits = xEventGroupWaitBits(
        this->m_even_group_handle,
        MQTT_PUBLISHED_BIT | MQTT_DISCONNECTED_BIT | MQTT_ERROR_BIT,
        pdTRUE,
        pdFALSE,
        pdMS_TO_TICKS(5000));

    if (bits & MQTT_PUBLISHED_BIT) return ESP_OK;
    return ESP_FAIL;
}

void MqttTransport::mqtt5_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    auto mqtt_client = static_cast<MqttTransport *>(handler_args);
    auto event = static_cast<esp_mqtt_event_t *>(event_data);


    switch (static_cast<esp_mqtt_event_id_t>(event_id)) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI("MQTT", "MQTT CONNECTED.");
            mqtt_client->publish();
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI("MQTT", "MQTT PUBLISHED.");
            xEventGroupSetBits(mqtt_client->m_even_group_handle, MQTT_PUBLISHED_BIT);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI("MQTT", "MQTT DISCONNECTED.");
            mqtt_client->disconnect();
            xEventGroupSetBits(mqtt_client->m_even_group_handle, MQTT_DISCONNECTED_BIT);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI("MQTT", "MQTT_EVENT_ERROR: %d", event->msg_id);
            xEventGroupSetBits(mqtt_client->m_even_group_handle, MQTT_ERROR_BIT);
            break;
        default:
            break;
    }
}

esp_err_t MqttTransport::disconnect() {
    return esp_mqtt_client_disconnect(this->m_client);
}

esp_err_t MqttTransport::publish() {
    int result = esp_mqtt_client_publish(this->m_client, "iot/weather", "test", 0, 1, 0); // WEATHER DATA IN FUTURE

    if (result < 1) {
        return ESP_FAIL;
    }
    return ESP_OK;
}
