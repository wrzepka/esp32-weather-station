//
// Created by Wiktor on 24.07.2026.
//

#include "MqttTransport.h"
#include "mqtt5_client.h"


esp_err_t MqttTransport::connect() {
    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.hostname = this->m_brokerIp;
    mqtt_cfg.broker.address.port = this->m_brokerPort;
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
    //TODO: memory leak fix.
    //TODO: async control (semaphor?)
    return result;
}

void MqttTransport::mqtt5_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    auto mqtt_client = static_cast<MqttTransport *>(handler_args);
    esp_mqtt_event_id_t event = static_cast<esp_mqtt_event_id_t>(event_id);

    switch (event) {
        case MQTT_EVENT_CONNECTED:
            //TODO: Implement
            break;
        case MQTT_EVENT_PUBLISHED:
            //TODO: Implement
            break;
        case MQTT_EVENT_DISCONNECTED:
            //TODO: Implement
            break;
        case MQTT_EVENT_ERROR:
            //TODO: Implement
            break;
        default:
            break;
    }
}

esp_err_t MqttTransport::disconnect() {
    return ESP_OK;
}

esp_err_t MqttTransport::publish() {
    return ESP_OK;
}
