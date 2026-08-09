//
// Created by Wiktor on 05.08.2026.
//

#include "../include/SensorManager.h"
#include <algorithm>
#include "freertos/FreeRTOS.h"

SensorManager::SensorManager(i2c_master_bus_handle_t i2c_master_bus_handle) {
    BME280::Config bme280_config = {};
    this->bh1750.begin(i2c_master_bus_handle);
    this->bme280.begin(i2c_master_bus_handle, bme280_config);
}

esp_err_t SensorManager::getPayload(Payload &payload) {
    bool bme280_start = false;
    bool bh1750_start = false;

    if (this->bh1750.trigger_measurement() == ESP_OK) bh1750_start = true;
    if (this->bme280.trigger_measurement() == ESP_OK) bme280_start = true;

    if (bme280_start && bh1750_start) return ESP_FAIL;

    vTaskDelay(pdMS_TO_TICKS(SensorManager::get_measurement_delay_in_ms()));

    bool bme280_measurement = false;
    bool bh1750_measurement = false;

    if (bh1750_start && (this->bh1750.fetch_measurement(payload.light_intensity) == ESP_OK)) {
        bh1750_measurement = true;
    } else {
        payload.light_intensity = UINT32_MAX;
    }

    if (bme280_start && (this->bme280.fetch_measurement(payload.bme_data) == ESP_OK)) {
        bme280_measurement = true;
    } else {
        payload.bme_data.humidity = UINT16_MAX;
        payload.bme_data.temperature = INT16_MAX;
        payload.bme_data.pressure = UINT32_MAX;
    }

    return (bh1750_measurement || bme280_measurement ? ESP_OK : ESP_FAIL);
}

constexpr uint32_t SensorManager::get_measurement_delay_in_ms() {
    return std::max({BME280::SUITABLE_MEASUREMENT_DELAY_IN_MS, BH1750::SUITABLE_MEASUREMENT_DELAY_IN_MS,});
}
