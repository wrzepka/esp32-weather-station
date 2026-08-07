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

SensorManager::Payload SensorManager::getPayload() { // TODO: change return type and use reference?
    SensorManager::Payload payload = {};

    if (this->bh1750.start_measurement() != ESP_OK || this->bme280.start_measurement() != ESP_OK) {
        return payload;
    }

    vTaskDelay(pdMS_TO_TICKS(180));

    if (this->bh1750.read_measurement(payload.light_intensity) != ESP_OK) {
        return payload;
    }

    if (this->bme280.read_measurement() != ESP_OK) { // TODO: use ref to fields? change method body.
        return payload;
    }

    return payload;
}