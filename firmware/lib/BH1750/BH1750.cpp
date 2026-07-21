//
// Created by Wiktor on 12.07.2026.
//

#include "BH1750.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2c_master.h"

esp_err_t BH1750::begin(i2c_master_bus_handle_t bus_handle) {
    if (bus_handle == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    const i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = m_address,
        .scl_speed_hz = 100000,
        .scl_wait_us = 0,
        .flags = {},
    };

    esp_err_t result = i2c_master_bus_add_device(bus_handle, &dev_config, &this->m_dev_handle);
    if (result != ESP_OK) {
        return result;
    }

    return ESP_OK;
}

esp_err_t BH1750::read_light_intensity(uint32_t &light_intensity) {
    esp_err_t result = i2c_master_transmit(this->m_dev_handle, &ONE_TIME_H_RESOLUTION_MODE, sizeof(uint8_t), pdMS_TO_TICKS(MAX_RESPONSE_TIME_IN_MS));
    if (result != ESP_OK) {
        return result;
    }

    vTaskDelay(pdMS_TO_TICKS(SUITABLE_MEASUREMENT_DELAY_IN_MS));
    uint8_t buffer[2] = {0};

    result = i2c_master_receive(this->m_dev_handle, buffer, sizeof(buffer), pdMS_TO_TICKS(MAX_RESPONSE_TIME_IN_MS));
    if (result != ESP_OK) {
        return result;
    }

    const auto light_intensity_float = static_cast<float>((static_cast<uint16_t>(buffer[0]) << 8) | buffer[1]);
    light_intensity = static_cast<uint32_t>(light_intensity_float / MEASURE_RATIO);

    return ESP_OK;
}
