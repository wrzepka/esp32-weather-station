//
// Created by Wiktor on 05.08.2026.
//

#include "SensorManager.h"

#include "freertos/FreeRTOS.h"

SensorManager::SensorManager(i2c_master_bus_handle_t i2c_master_bus_handle) {
    BME280::Config bme280_config = {};
    this->bh1750.begin(i2c_master_bus_handle);
    this->bme280.begin(i2c_master_bus_handle, bme280_config);
}

esp_err_t SensorManager::getPayload(Payload& payload) {
    bool bme280_start = false;
    bool bh1750_start = false;

    if (this->bh1750.start_measurement() == ESP_OK) bh1750_start = true;
    if (this->bme280.start_measurement() == ESP_OK) bme280_start = true;

    if (bme280_start && bh1750_start) return ESP_FAIL;

    vTaskDelay(pdMS_TO_TICKS(180));

    bool bme280_measurement = false;
    bool bh1750_measurement = false;

    if (bh1750_start && (this->bh1750.read_measurement(payload.light_intensity) == ESP_OK)) {
        bh1750_measurement = true;
    } else {
        payload.light_intensity = UINT32_MAX;
    }

    if (bme280_start && (this->bme280.read_measurement(payload.bme_data) == ESP_OK)) {
        bme280_measurement = true;
    } else {
        payload.bme_data.humidity = UINT32_MAX;
        payload.bme_data.temperature = INT32_MAX;
        payload.bme_data.pressure = UINT32_MAX;
    }

    return (bh1750_measurement || bme280_measurement ? ESP_OK : ESP_FAIL);
}