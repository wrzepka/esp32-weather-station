//
// Created by Wiktor on 13.07.2026.
//

#include "BME280.h"

#include <esp_log.h>
#include <driver/gpio.h>
#include <driver/i2c_master.h>

bool BME280::begin(i2c_master_bus_handle_t bus_handle) {
    const i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = _address,
        .scl_speed_hz = 100000,
        .scl_wait_us = 0,
        .flags = {},
    };

    if (i2c_master_bus_add_device(bus_handle, &dev_config, &this->_dev_handle) == ESP_OK) {
        return true;
    }
    return false;
}

const BME280::bme280_calib_data &BME280::calib_data() const {
    return _calib_data;
}


bool BME280::read_calib_data() {
    uint8_t data[26] = {0};
    uint8_t reg_address = 0x88;

    esp_err_t err = i2c_master_transmit_receive(this->_dev_handle, &reg_address, 1, data, sizeof(data), -1);
    if (err != ESP_OK) {
        gpio_set_level(GPIO_NUM_15, 1);
        return false;
    }


    this->_calib_data.dig_T1 = data[0] | (data[1] << 8);
    this->_calib_data.dig_T2 = static_cast<int16_t>(data[2] | (data[3] << 8));
    this->_calib_data.dig_T3 = static_cast<int16_t>(data[4] | (data[5] << 8));
    //TODO: Set up rest of calibration data

    return true;
}
