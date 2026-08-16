/**
 * @file SensorManager.h
 * @author Wiktor
 * @brief Class for handling management of weather station sensors.
 * @date 05.08.2026
 **/
#ifndef FIRMWARE_SENSORMANAGER_H
#define FIRMWARE_SENSORMANAGER_H
#include "BH1750.h"
#include "BME280.h"

/**
 * @class SensorManager
 * @brief Class for managing weather station sensors.
 *
 * This class provides sensors initialization, measurement triggering and payload creating.
 */
class SensorManager {
private:
    BME280 bme280; /**<Sensor measuring temperature, pressure and humidity.*/
    BH1750 bh1750; /**<Sensor measuring light intensity.*/
public:
    /**
     * @brief structure used for transferring sensor data via transport protocol.
     */
    struct Payload {
        uint32_t light_intensity;
        BME280::Data bme_data;
    };

    /**
     * @brief Initializes sensors.
     *
     * Initializes all sensors (BH1750, BME280) by using begin() methods.
     *
     * @param i2c_master_bus_handle I2C master bus handle.
     */
    SensorManager(i2c_master_bus_handle_t i2c_master_bus_handle);

    /**
     * @brief triggers measurements and then fetches data.
     *
     * Triggers sensors measurement. If at least one measurement succeed, fetches data from that sensor and saves it to
     * Payload structure. Otherwise, fill Payload fields with value sentinels (ex. INT_32_MAX).
     *
     * @param[out] payload reference to Payload structure. It contains measurement data.
     * @return ESP_OK if fetches data from at least one sensor.
     * @return ESP_FAIL if all sensors are down.
     */
    esp_err_t getPayload(Payload &payload);

    /**
     * @brief Compares and returns the highest measurement delay.
     *
     * @return the highest measurement delay.
     */
    static constexpr uint32_t get_measurement_delay_in_ms();
};

#endif //FIRMWARE_SENSORMANAGER_H
