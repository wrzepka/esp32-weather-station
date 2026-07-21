//
// Created by Wiktor on 13.07.2026.
//
#include <cstdio>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "WiFiManager.h"
#include "driver/i2c_master.h"
#include "../lib/BH1750/BH1750.h"
#include "../lib/BME280/BME280.h"

#define BLINK_GPIO GPIO_NUM_15

esp_err_t init_i2c();
void i2c_scanner(i2c_master_bus_handle_t bus_handle);

static const char *TAG = "FireBeetle 2 ESP32-C6 Weather Station";

extern "C" void app_main(void) {
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    ESP_ERROR_CHECK(init_i2c());
    i2c_master_bus_handle_t i2c_bus_handle;
    i2c_master_get_bus_handle(I2C_NUM_0, &i2c_bus_handle);

    auto *bh1750 = new BH1750();
    auto *bme280 = new BME280();
    auto *wifi_manager = new WiFiManager();

    uint8_t led_state = 0;
    uint32_t light_intensity = 0;
    wifi_manager->init_wifi_station();
    ESP_ERROR_CHECK(bh1750->begin(i2c_bus_handle));
    BME280::Config bme280_config = {};
    ESP_ERROR_CHECK(bme280->begin(i2c_bus_handle, bme280_config));

    while (true) {
        i2c_scanner(i2c_bus_handle);

        led_state = !led_state;
        gpio_set_level(BLINK_GPIO, led_state);

        esp_err_t result = bh1750->read_light_intensity(light_intensity);

        if (result != ESP_OK) {
            ESP_LOGE(TAG, "Failed to read light intensity: %s", esp_err_to_name(result));
        }else {
            ESP_LOGI(TAG, "ILLUMINANCE: %u", light_intensity);
        }

        result = bme280->read_weather_data();
        if (result != ESP_OK) {
            ESP_LOGE(TAG, "Failed to read BME280 sensor data: %s", esp_err_to_name(result));
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
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
