//
// Created by Wiktor on 13.08.2026.
//

#include "WeatherStation.h"

#include <esp_log.h>

#include "esp_sleep.h"
#include "JsonPayloadSerializer.h"

static auto TAG = "WeatherStation";

esp_err_t WeatherStation::init_sensors() {
    return sensor_manager.init_sensors();
};

esp_err_t WeatherStation::init_communication() {
    esp_err_t result = wifi_manager.init_wifi_station();
    if (result != ESP_OK) {
        return result;
    }

    return transport.connect();
};

esp_err_t WeatherStation::measure() {
    return sensor_manager.getPayload(current_payload);
}

esp_err_t WeatherStation::send() {
    JsonPayloadSerializer payload_serializer;
    std::string json;

    esp_err_t serialization_result = payload_serializer.serialize(current_payload, json);
    if (serialization_result != ESP_OK) {
        return serialization_result;
    }

    esp_err_t init_communication_result = init_communication();
    if (init_communication_result != ESP_OK) {
        return init_communication_result;
    }

    return transport.publish(json);
}

esp_err_t WeatherStation::sleep() {
    transport.disconnect();
    wifi_manager.deinit_wifi_station();

    esp_err_t sleep_result = ESP_OK;
    sleep_result = esp_sleep_enable_timer_wakeup(minutes_to_us(deep_sleep_length_in_minutes));
    if (sleep_result != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure wakeup timer: %s", esp_err_to_name(sleep_result));
        return sleep_result;
    }

    esp_deep_sleep_start();

    return ESP_OK; //unreachable code, but required by compilator? (I guess so)
}
