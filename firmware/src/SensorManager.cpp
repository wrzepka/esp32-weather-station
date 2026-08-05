//
// Created by Wiktor on 05.08.2026.
//

#include "SensorManager.h"

SensorManager::SensorManager(i2c_master_bus_handle_t i2c_master_bus_handle) {
    BME280::Config bme280_config = {};
    this->bh1750.begin(i2c_master_bus_handle);
    this->bme280.begin(i2c_master_bus_handle, bme280_config);
}

SensorManager::Payload SensorManager::getPayload() {
    SensorManager::Payload payload = {};


    return payload;
}