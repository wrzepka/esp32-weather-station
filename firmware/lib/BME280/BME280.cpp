//
// Created by Wiktor on 13.07.2026.
//

#include "BME280.h"

#include <esp_log.h>
#include <driver/gpio.h>
#include <driver/i2c_master.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

esp_err_t BME280::begin(i2c_master_bus_handle_t bus_handle) {
    if (bus_handle == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    const i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = _address,
        .scl_speed_hz = 100000,
        .scl_wait_us = 0,
        .flags = {},
    };

    esp_err_t result = i2c_master_bus_add_device(bus_handle, &dev_config, &this->_dev_handle);
    if (result != ESP_OK) {
        return result;
    }

    bme_280_hum_meas hum_meas = {
        .fields = {.osrs_h = 1, .RESERVED = 0}
    };

    bme_280_ctrl_meas ctrl_meas = {
        .fields = {
            .mode = 0,
            .osrs_p = 1,
            .osrs_t = 1,
        }
    };

    uint8_t transmit_data[2] = {REG_CONTROL_HUM_ADDR, hum_meas.raw};
    result = i2c_master_transmit(this->_dev_handle, transmit_data, sizeof(transmit_data),
                                 pdMS_TO_TICKS(MAX_RESPONSE_TIME_IN_MS));
    if (result != ESP_OK) {
        return result;
    }

    transmit_data[0] = REG_CONTROL_MEAS_ADDR;
    transmit_data[1] = ctrl_meas.raw;
    result = i2c_master_transmit(this->_dev_handle, transmit_data, sizeof(transmit_data),
                                 pdMS_TO_TICKS(MAX_RESPONSE_TIME_IN_MS));
    if (result != ESP_OK) {
        return result;
    }

    return ESP_OK;
}

const BME280::bme280_calib_data &BME280::calib_data() const {
    return _calib_data;
}

esp_err_t BME280::read_calib_data() {
    uint8_t data[CALIB_00_PAYLOAD_SIZE] = {0};

    esp_err_t result = i2c_master_transmit_receive(this->_dev_handle, &REG_CALIB_00_ADDR, sizeof(REG_CALIB_00_ADDR),
                                                   data,
                                                   CALIB_00_PAYLOAD_SIZE, pdMS_TO_TICKS(MAX_RESPONSE_TIME_IN_MS));
    if (result != ESP_OK) {
        return result;
    }

    this->_calib_data.dig_T1 = data[0] | (data[1] << 8);
    this->_calib_data.dig_T2 = static_cast<int16_t>(data[2] | (data[3] << 8));
    this->_calib_data.dig_T3 = static_cast<int16_t>(data[4] | (data[5] << 8));

    this->_calib_data.dig_P1 = data[6] | (data[7] << 8);
    this->_calib_data.dig_P2 = static_cast<int16_t>(data[8] | (data[9] << 8));
    this->_calib_data.dig_P3 = static_cast<int16_t>(data[10] | (data[11] << 8));
    this->_calib_data.dig_P4 = static_cast<int16_t>(data[12] | (data[13] << 8));
    this->_calib_data.dig_P5 = static_cast<int16_t>(data[14] | (data[15] << 8));
    this->_calib_data.dig_P6 = static_cast<int16_t>(data[16] | (data[17] << 8));
    this->_calib_data.dig_P7 = static_cast<int16_t>(data[18] | (data[19] << 8));
    this->_calib_data.dig_P8 = static_cast<int16_t>(data[20] | (data[21] << 8));
    this->_calib_data.dig_P9 = static_cast<int16_t>(data[22] | (data[23] << 8));

    this->_calib_data.dig_H1 = data[25];

    result = i2c_master_transmit_receive(this->_dev_handle, &REG_CALIB_26_ADDR, sizeof(REG_CALIB_26_ADDR), data,
                                         CALIB_26_PAYLOAD_SIZE, pdMS_TO_TICKS(MAX_RESPONSE_TIME_IN_MS));
    if (result != ESP_OK) {
        return result;
    }

    this->_calib_data.dig_H2 = static_cast<int16_t>(data[0] | (data[1] << 8));
    this->_calib_data.dig_H3 = data[2];
    this->_calib_data.dig_H4 = static_cast<int16_t>((data[3] << 4) | ((data[4] & 0xF)));
    if (this->_calib_data.dig_H4 & 0x800) this->_calib_data.dig_H4 |= static_cast<int16_t>(0xF000);

    this->_calib_data.dig_H5 = static_cast<int16_t>((data[4] >> 4) | (data[5] << 4));
    if (this->_calib_data.dig_H5 & 0x800) this->_calib_data.dig_H5 |= static_cast<int16_t>(0xF000);
    this->_calib_data.dig_H6 = static_cast<int8_t>(data[6]);

    return ESP_OK;
}

void BME280::print_calib_data() const {
    const char *TAG = "BME280";
    ESP_LOGI(TAG, "T1 CALIB DATA: %u", this->_calib_data.dig_T1);
    ESP_LOGI(TAG, "T2 CALIB DATA: %d", this->_calib_data.dig_T2);
    ESP_LOGI(TAG, "T3 CALIB DATA: %d", this->_calib_data.dig_T3);
    ESP_LOGI(TAG, "P1 CALIB DATA: %d", this->_calib_data.dig_P1);
    ESP_LOGI(TAG, "P2 CALIB DATA: %d", this->_calib_data.dig_P2);
    ESP_LOGI(TAG, "P3 CALIB DATA: %d", this->_calib_data.dig_P3);
    ESP_LOGI(TAG, "P4 CALIB DATA: %d", this->_calib_data.dig_P4);
    ESP_LOGI(TAG, "P5 CALIB DATA: %d", this->_calib_data.dig_P5);
    ESP_LOGI(TAG, "P6 CALIB DATA: %d", this->_calib_data.dig_P6);
    ESP_LOGI(TAG, "P7 CALIB DATA: %d", this->_calib_data.dig_P7);
    ESP_LOGI(TAG, "P8 CALIB DATA: %d", this->_calib_data.dig_P8);
    ESP_LOGI(TAG, "P9 CALIB DATA: %d", this->_calib_data.dig_P9);
    ESP_LOGI(TAG, "H1 CALIB DATA: %d", this->_calib_data.dig_H1);
    ESP_LOGI(TAG, "H2 CALIB DATA: %d", this->_calib_data.dig_H2);
    ESP_LOGI(TAG, "H3 CALIB DATA: %d", this->_calib_data.dig_H3);
    ESP_LOGI(TAG, "H4 CALIB DATA: %d", this->_calib_data.dig_H4);
    ESP_LOGI(TAG, "H5 CALIB DATA: %d", this->_calib_data.dig_H5);
    ESP_LOGI(TAG, "H6 CALIB DATA: %d", this->_calib_data.dig_H6);
}

int32_t BME280::compensate_temperature(int32_t adc_temp) {
    int32_t var1, var2, temp;
    var1 = (((adc_temp >> 3) - (this->_calib_data.dig_T1 << 1)) * this->_calib_data.dig_T2) >> 11;
    var2 = ((((adc_temp >> 4) - (this->_calib_data.dig_T1)) * ((adc_temp >> 4) - this->_calib_data.dig_T1)) >> 12) *
           this->_calib_data.dig_T3 >> 14;

    this->fine_temp = var1 + var2;
    temp = (this->fine_temp * 5 + 128) >> 8;
    return temp;
}

uint32_t BME280::compensate_pressure(int32_t adc_p) {
    int32_t var1, var2;
    uint32_t press;

    var1 = (this->fine_temp >> 1) - 64000;
    var2 = ((var1 >> 2) * (var1 >> 2) >> 11) * this->_calib_data.dig_P6;
    var2 = var2 + ((var1 * this->_calib_data.dig_P5) << 1);
    var2 = (var2 >> 2) + (static_cast<int32_t>(this->_calib_data.dig_P4) << 16);
    var1 = (((this->_calib_data.dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + (
                (static_cast<int32_t>(this->_calib_data.dig_P2) * var1) >> 1)) >> 18;
    var1 = ((32768 + var1) * this->_calib_data.dig_P1) >> 15;

    if (var1 == 0) {
        return 0;
    }
    press = ((1048576 - adc_p) - (var2 >> 12)) * 3125;
    if (press < 0x80000000) {
        press = (press << 1) / var1;
    } else {
        press = (press / var1) * 2;
    }

    var1 = ((static_cast<int32_t>(this->_calib_data.dig_P9) * (
                 static_cast<int32_t>((press >> 3) * (press >> 3)) >> 13))) >> 12;
    var2 = (static_cast<int32_t>(press >> 2) * static_cast<int32_t>(this->_calib_data.dig_P8)) >> 13;

    press = static_cast<uint32_t>(static_cast<int32_t>(press) + ((var1 + var2 + this->_calib_data.dig_P7) >> 4));
    return press;
}

uint32_t BME280::compensate_humidity(int32_t adc_H) {
    int32_t v_x1;

    v_x1 = (this->fine_temp - static_cast<int32_t>(76800));

    v_x1 = (((((adc_H << 14) - (static_cast<int32_t>(this->_calib_data.dig_H4) << 20) -
               (static_cast<int32_t>(this->_calib_data.dig_H5) * v_x1)) + static_cast<int32_t>(16384)) >> 15) *
            (((((((v_x1 * static_cast<int32_t>(this->_calib_data.dig_H6)) >> 10) *
                 (((v_x1 * static_cast<int32_t>(this->_calib_data.dig_H3)) >> 11) +
                  static_cast<int32_t>(32768))) >> 10) + static_cast<int32_t>(2097152)) *
              static_cast<int32_t>(this->_calib_data.dig_H2) + 8192) >> 14));


    v_x1 = (v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * (static_cast<int32_t>(this->_calib_data.dig_H1))) >> 4));

    v_x1 = (v_x1 < 0 ? 0 : v_x1);
    v_x1 = (v_x1 > 419430400 ? 419430400 : v_x1);

    return static_cast<uint32_t>(v_x1 >> 12);
}

esp_err_t BME280::read_weather_data() {
    uint8_t transmit_data[2] = {REG_CONTROL_MEAS_ADDR, 0x25};
    esp_err_t result = i2c_master_transmit(this->_dev_handle, transmit_data, sizeof(transmit_data),
                                           pdMS_TO_TICKS(MAX_RESPONSE_TIME_IN_MS));
    if (result != ESP_OK) {
        return result;
    }

    vTaskDelay(pdMS_TO_TICKS(SUITABLE_MEASUREMENT_DELAY_IN_MS));

    uint8_t data[8] = {0};

    result = i2c_master_transmit_receive(this->_dev_handle, &REG_MEAS_DATA_START_ADDR,
                                                sizeof(REG_MEAS_DATA_START_ADDR), data, MEAS_DATA_PAYLOAD_SIZE,
                                                pdMS_TO_TICKS(MAX_RESPONSE_TIME_IN_MS));
    if (result != ESP_OK) {
        return result;
    }

    int32_t adc_T, adc_P, adc_H;

    adc_T = (data[3] << 12) | (data[4] << 4) | ((data[5] >> 4));
    int32_t T = compensate_temperature(adc_T);

    adc_H = (data[6] << 8) | (data[7]);
    uint32_t H = compensate_humidity(adc_H); // need to divide by 1024 to get Q22.10 format

    // THIS IS ABSOLUTE PRESSURE
    adc_P = (data[0] << 12) | (data[1] << 4) | ((data[2] >> 4));
    uint32_t P = compensate_pressure(adc_P);


    //TEMPORARY SOLUTION
    ESP_LOGI("BME280", "Temperature: %d", T);
    ESP_LOGI("BME280", "Pressure: %u", P);
    ESP_LOGI("BME280", "Humidity ABS: %u || %.2f%:", H, (static_cast<float>(H)/1024));

    return ESP_OK;
}
