//
// Created by Wiktor on 05.08.2026.
//

#include "../include/SensorManager.h"
#include <algorithm>
#include "freertos/FreeRTOS.h"
#include "esp_check.h"

static auto TAG = "SensorManager";

esp_err_t SensorManager::init_sensors() {
    BME280::Config bme280_config = {};

    ESP_RETURN_ON_ERROR(this->bh1750.begin(this->i2c_bus_handle), TAG, "BH1750 initialization failed.");
    ESP_RETURN_ON_ERROR(this->bme280.begin(this->i2c_bus_handle, bme280_config), TAG, "BME280 initialization failed.");

    return ESP_OK;
}


esp_err_t SensorManager::getPayload(Payload &payload) {
    bool bme280_start = true;
    bool bh1750_start = true;

    esp_err_t bh1750_result = ESP_OK;
    esp_err_t bme280_result = ESP_OK;

    if ((bh1750_result = this->bh1750.trigger_measurement()) != ESP_OK) {
        ESP_LOGW(TAG, "BH1750 measurement trigger failed: %s", esp_err_to_name(bh1750_result));
        payload.light_intensity = UINT32_MAX;
        bh1750_start = false;
    }

    if ((bme280_result =  this->bme280.trigger_measurement()) != ESP_OK) {
        ESP_LOGW(TAG, "BME280 measurement trigger failed: %s", esp_err_to_name(bme280_result));
        payload.bme_data.humidity = UINT16_MAX;
        payload.bme_data.temperature = INT16_MAX;
        payload.bme_data.pressure = UINT32_MAX;
        bme280_start = false;
    }

    if (!bme280_start && !bh1750_start) {
        ESP_LOGE(TAG, "Couldn't fetch any data!");
        return ESP_FAIL;
    }
    vTaskDelay(pdMS_TO_TICKS(SensorManager::get_measurement_delay_in_ms()));

    bool bme280_measurement = false;
    bool bh1750_measurement = false;

    if (bh1750_start && ((bh1750_result = this->bh1750.fetch_measurement(payload.light_intensity)) == ESP_OK)) {
        bh1750_measurement = true;
    }else {
        payload.light_intensity = UINT32_MAX;
    }

    if (bme280_start && ((bme280_result = this->bme280.fetch_measurement(payload.bme_data)) == ESP_OK)) {
        bme280_measurement = true;
    } else {
        payload.bme_data.humidity = UINT16_MAX;
        payload.bme_data.temperature = INT16_MAX;
        payload.bme_data.pressure = UINT32_MAX;
    }

    if (!bme280_measurement && bme280_start) ESP_LOGW(TAG, "Couldn't fetch measurement from BME280: %s", esp_err_to_name(bme280_result));
    if (!bh1750_measurement && bh1750_start) ESP_LOGW(TAG, "Couldn't fetch measurement from BH1750: %s", esp_err_to_name(bh1750_result));

    return (bh1750_measurement || bme280_measurement ? ESP_OK : ESP_FAIL);
}

constexpr uint32_t SensorManager::get_measurement_delay_in_ms() {
    return std::max({BME280::SUITABLE_MEASUREMENT_DELAY_IN_MS, BH1750::SUITABLE_MEASUREMENT_DELAY_IN_MS,});
}
