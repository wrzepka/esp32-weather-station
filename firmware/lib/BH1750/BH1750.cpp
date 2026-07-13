//
// Created by Wiktor on 12.07.2026.
//

#include "BH1750.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2c_master.h"

bool BH1750::begin(i2c_master_bus_handle_t bus_handle) {
    const i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = _address,
        .scl_speed_hz = 100000,
        .scl_wait_us = 0,
        .flags = {},
    };

    return i2c_master_bus_add_device(bus_handle, &dev_config, &this->_dev_handle) == ESP_OK;
}

uint32_t BH1750::read_light_intensity() const {
    i2c_master_transmit(this->_dev_handle, &ONE_TIME_H_RESOLUTION_MODE, sizeof(uint8_t), -1);

    vTaskDelay(pdMS_TO_TICKS(180));

    uint8_t buffer[2] = {0};

    i2c_master_receive(this->_dev_handle, buffer, sizeof(buffer), pdMS_TO_TICKS(1000));

    float light_intensity = (static_cast<uint16_t>(buffer[0]) << 8) | buffer[1];
    light_intensity = light_intensity / 1.2;

    return static_cast<uint32_t>(light_intensity);
}
