//
// Created by Wiktor on 11.08.2026.
//

#include <JsonPayloadSerializer.h>
#include <string>

#include "cJSON.h"
#include "../managed_components/espressif__cjson/cJSON/cJSON.h"

esp_err_t JsonPayloadSerializer::serialize(const SensorManager::Payload& payload, std::string& serialize_string) {
    cJSON *object = cJSON_CreateObject();

    if (object == nullptr) return ESP_ERR_NO_MEM;

    cJSON_AddNumberToObject(object, "light_intensity", payload.light_intensity);
    cJSON_AddNumberToObject(object, "humidity", payload.bme_data.humidity);
    cJSON_AddNumberToObject(object, "temperature", payload.bme_data.temperature);
    cJSON_AddNumberToObject(object, "pressure", payload.bme_data.pressure);

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