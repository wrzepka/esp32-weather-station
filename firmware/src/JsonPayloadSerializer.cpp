//
// Created by Wiktor on 11.08.2026.
//

#include <esp_log.h>
#include <JsonPayloadSerializer.h>
#include <string>

#include "cJSON.h"
#include "../managed_components/espressif__cjson/cJSON/cJSON.h"

esp_err_t JsonPayloadSerializer::serialize(const SensorManager::Payload& payload, std::string& serialize_string) {
    static auto TAG = "JsonPayloadSerializer";

    cJSON *object = cJSON_CreateObject();

    if (object == nullptr) return ESP_ERR_NO_MEM;

    // Auxiliary lambda function for adding new fields to the cJSON object.
    auto add_field = [&object](const char* name, auto value, auto error_value) -> bool {
        cJSON* item = (value == error_value) ? cJSON_AddNullToObject(object, name) : cJSON_AddNumberToObject(object, name, value);

        return item != nullptr;
    };

    bool success = add_field("light_intensity", payload.light_intensity, UINT32_MAX)
                && add_field("humidity", payload.bme_data.humidity, UINT16_MAX)
                && add_field("temperature", payload.bme_data.temperature, INT16_MAX)
                && add_field("pressure", payload.bme_data.pressure, UINT32_MAX);

    if (success == false) {
        cJSON_Delete(object);
        ESP_LOGE(TAG, "Failed to build telemetry JSON payload");
        return ESP_ERR_NO_MEM;
    }

    char *raw_json = cJSON_PrintUnformatted(object);
    if (raw_json == nullptr) {
        cJSON_Delete(object);
        return ESP_ERR_NO_MEM;
    }

    serialize_string = raw_json;
    cJSON_Delete(object);
    cJSON_free(raw_json);

    return ESP_OK;
}