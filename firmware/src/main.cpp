//
// Created by Wiktor on 13.07.2026.
//
#include <cstdio>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "WeatherStation.h"
#include "../include/MqttTransport.h"
#include "WiFiManager.h"
#include "driver/i2c_master.h"
#include "../lib/BH1750/BH1750.h"
#include "../lib/BME280/BME280.h"

#define BLINK_GPIO GPIO_NUM_15

esp_err_t init_i2c();

void i2c_scanner(i2c_master_bus_handle_t bus_handle);

static const char *TAG = "FireBeetle 2 ESP32-C6 Weather Station";

extern "C" void app_main(void) {
    // gpio_reset_pin(BLINK_GPIO);
    // gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    ESP_ERROR_CHECK(init_i2c());
    i2c_master_bus_handle_t i2c_bus_handle;
    i2c_master_get_bus_handle(I2C_NUM_0, &i2c_bus_handle);

    MqttTransport transport("10.246.161.98", 1883);
    WeatherStation station(i2c_bus_handle, transport);

    esp_err_t operation_result = ESP_OK;
    if (((operation_result = station.init_sensors()) != ESP_OK) ||
        ((operation_result = station.measure()) != ESP_OK) ||
        ((operation_result = station.init_communication()) != ESP_OK) ||
        ((operation_result = station.send()) != ESP_OK)) {
        ESP_LOGE(TAG, "Critical sequence failure (%s). Forcing deep sleep.", esp_err_to_name(operation_result));
    }
    station.sleep();
}

esp_err_t init_i2c() {
    i2c_master_bus_handle_t bus_handle;

    constexpr i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = GPIO_NUM_19,
        .scl_io_num = GPIO_NUM_20,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .trans_queue_depth = 0,
        .flags = {},
    };

    return i2c_new_master_bus(&bus_config, &bus_handle);
}

void i2c_scanner(i2c_master_bus_handle_t bus_handle) {
    for (uint8_t address = 0x08; address < 0x78; address++) {
        esp_err_t check_result = i2c_master_probe(bus_handle, address, -1);
        if (check_result == ESP_OK) {
            ESP_LOGI(TAG, "Found device at 0x%02X", address);
        }
    }
}
