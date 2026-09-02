//
// Created by Wiktor on 24.07.2026.
//

#include "../include/MqttTransport.h"

#include <esp_check.h>
#include <esp_log.h>
#include "esp_mac.h"

#include "mqtt_client.h"

static auto TAG = "MqttTransport";

esp_err_t MqttTransport::connect() {
    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.hostname = this->m_brokerIp;
    mqtt_cfg.broker.address.port = this->m_brokerPort;
    mqtt_cfg.broker.address.transport = MQTT_TRANSPORT_OVER_TCP;
    mqtt_cfg.session.protocol_ver = MQTT_PROTOCOL_V_5;
    mqtt_cfg.session.disable_clean_session = false;

    esp_mqtt5_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    if (client == nullptr) {
        ESP_LOGE(TAG, "Failed to initialize MQTT client.");
        return ESP_FAIL;
    }

    this->m_client = client;

    esp_err_t result = ESP_OK;
    result = esp_mqtt_client_register_event(client, static_cast<esp_mqtt_event_id_t>(ESP_EVENT_ANY_ID),
                                            mqtt5_event_handler, this);
    if (result != ESP_OK) {
        esp_mqtt_client_destroy(this->m_client);
        this->m_client = nullptr;

        ESP_LOGE(TAG, "Failed to register MQTT event.");
        return result;
    }

    xEventGroupClearBits(this->m_event_group_handle, MQTT_DISCONNECTED_BIT | MQTT_CONNECTED_BIT | MQTT_ERROR_BIT);
    result = esp_mqtt_client_start(client);
    if (result != ESP_OK) {
        esp_mqtt_client_destroy(this->m_client);
        this->m_client = nullptr;

        ESP_LOGE(TAG, "Failed to start MQTT client.");
        return result;
    }

    EventBits_t bits = xEventGroupWaitBits(
        this->m_event_group_handle,
        MQTT_CONNECTED_BIT | MQTT_DISCONNECTED_BIT | MQTT_ERROR_BIT,
        pdTRUE,
        pdFALSE,
        pdMS_TO_TICKS(MAX_EVENT_GROUP_WAIT_TIME));

    if (bits & MQTT_CONNECTED_BIT) return ESP_OK;

    esp_mqtt_client_stop(this->m_client);
    esp_mqtt_client_destroy(this->m_client);
    this->m_client = nullptr;

    ESP_LOGE(TAG, "Failed to connect to the MQTT broker.");

    return ESP_FAIL;
}

void MqttTransport::mqtt5_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    auto mqtt_client = static_cast<MqttTransport *>(handler_args);
    auto event = static_cast<esp_mqtt_event_t *>(event_data);


    switch (static_cast<esp_mqtt_event_id_t>(event_id)) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT CONNECTED.");
            xEventGroupSetBits(mqtt_client->m_event_group_handle, MQTT_CONNECTED_BIT);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT PUBLISHED.");
            xEventGroupSetBits(mqtt_client->m_event_group_handle, MQTT_PUBLISHED_BIT);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT DISCONNECTED.");
            xEventGroupSetBits(mqtt_client->m_event_group_handle, MQTT_DISCONNECTED_BIT);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR: %d", event->msg_id);
            xEventGroupSetBits(mqtt_client->m_event_group_handle, MQTT_ERROR_BIT);
            break;
        default:
            break;
    }
}

esp_err_t MqttTransport::disconnect() {
    if (this->m_client == nullptr) {
        return ESP_OK;
    }

    xEventGroupClearBits(this->m_event_group_handle, MQTT_DISCONNECTED_BIT | MQTT_ERROR_BIT);

    esp_err_t result = esp_mqtt_client_disconnect(this->m_client);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "Failed to disconnect from the broker.");
        return result;
    }

    EventBits_t bits = xEventGroupWaitBits(
        this->m_event_group_handle,
        MQTT_DISCONNECTED_BIT | MQTT_ERROR_BIT,
        pdTRUE,
        pdFALSE,
        pdMS_TO_TICKS(MAX_EVENT_GROUP_WAIT_TIME));

    esp_mqtt_client_stop(this->m_client);
    esp_mqtt_client_destroy(this->m_client);
    this->m_client = nullptr;

    if (bits & MQTT_DISCONNECTED_BIT) {
        return ESP_OK;
    }

    ESP_LOGE(TAG, "Failed to disconnect from the broker.");
    return ESP_ERR_TIMEOUT;
}

esp_err_t MqttTransport::publish(const std::string& serializedPayload) {
    if (this->m_client == nullptr) {
        ESP_LOGE(TAG, "Couldn't publish a message because of lack of the esp_mqtt_client_handle_t.");
        return ESP_ERR_INVALID_STATE;
    }

    xEventGroupClearBits(this->m_event_group_handle, MQTT_PUBLISHED_BIT | MQTT_ERROR_BIT | MQTT_DISCONNECTED_BIT);

    std::string topic = generate_topic();
    if (topic.empty()) {
        ESP_LOGE(TAG, "Failed to publish a message.");
        return ESP_ERR_INVALID_STATE;
    }

    int result = esp_mqtt_client_publish(this->m_client, topic.c_str(), serializedPayload.c_str(), static_cast<int>(serializedPayload.length()), QOS, 0);

    if (result < 0) {
        ESP_LOGE(TAG, "Failed to publish a message.");
        return ESP_FAIL;
    }

    EventBits_t bits = xEventGroupWaitBits(
        this->m_event_group_handle,
        MQTT_PUBLISHED_BIT | MQTT_ERROR_BIT | MQTT_DISCONNECTED_BIT,
        pdTRUE,
        pdFALSE,
        pdMS_TO_TICKS(MAX_EVENT_GROUP_WAIT_TIME));

    if (bits & MQTT_PUBLISHED_BIT) {
        return ESP_OK;
    }

    ESP_LOGE(TAG, "Failed to publish a message.");
    return ESP_FAIL;
}

std::string MqttTransport::generate_topic() {
    uint8_t mac_address[6];
    esp_err_t status = esp_read_mac(mac_address, ESP_MAC_WIFI_STA);

    char topic[64] = {}; //TODO: calc array size;

    if (status == ESP_OK) {
        int result = snprintf(topic, sizeof(topic), "%s%s_%02X-%02X-%02X-%02X-%02X-%02X", TOPIC_BASE , m_device_id, mac_address[0], mac_address[1], mac_address[2], mac_address[3], mac_address[4], mac_address[5]);

        if (result > 0 && result < sizeof(topic)) {
            return topic;
        }
    }
    return {};
}
