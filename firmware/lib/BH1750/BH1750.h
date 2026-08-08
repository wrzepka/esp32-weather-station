/**
 * @file BH1750.h
 * @author Wiktor
 * @brief Driver library for BH1750 sensor using ESP-IDF I2C master driver.
 * @date 2026-07-12
 *
 * @note Register addresses, timing and lux conversion factor are based on the
 *       official ROHM BH1750FVI datasheet.
 */

#ifndef FIRMWARE_BH1750_H
#define FIRMWARE_BH1750_H
#include <esp_err.h>
#include <driver/i2c_types.h>

/**
 * @class BH1750
 * @brief Class for interaction with BH1750 illuminance sensor.
 *
 * This class provides basic control over BH1750 illuminance sensor.
 * It handles initialization and readout of illuminance.
 */
class BH1750 {
public:
    static constexpr uint8_t DEFAULT_I2C_ADDRESS = 0x23; /**< Default I2C address for BH1750 sensor.*/
    static constexpr uint8_t SECONDARY_I2C_ADDRESS = 0x5c;
    /**< Secondary I2C address for BH1750 sensor if ADDR is HIGH.*/
private:
    uint8_t m_address; /**< I2C device address*/
    i2c_master_dev_handle_t m_dev_handle; /**< I2C device handle*/
    static constexpr uint8_t ONE_TIME_H_RESOLUTION_MODE = 0x20; /**< Value for high resolution one time measurement*/
    static constexpr float MEASURE_RATIO = 1.2f;
    /** Ratio that is used for calculating proper illuminance. Value is from docs.*/
    static constexpr uint32_t SUITABLE_MEASUREMENT_DELAY_IN_MS = 180; /**< Appropriate delay for proper measurement. */
    static constexpr uint32_t MAX_RESPONSE_TIME_IN_MS = 100; /** Maximum waiting time for response in ms.*/
public:
    BH1750(const uint8_t address = DEFAULT_I2C_ADDRESS) : m_address(address), m_dev_handle(nullptr) {
    }

    /**
     * @brief Initializes I2C connection for BH1750 sensor.
     *
     * Configures device handler and adds it to the I2C bus.
     *
     * @param bus_handle I2C master bus handle.
     * @return ESP_ERR_INVALID_ARG if bus handle is nullptr.
     * @return ESP_OK if initialization succeed.
     * @return Other esp errors if something went wrong.
     */
    esp_err_t begin(i2c_master_bus_handle_t bus_handle);

    /**
     * @brief Makes a I/O blocking light intensity measurement.
     *
     * Demands measurement start via I2C bus. Waits SUITABLE_MEASUREMENT_DELAY_IN_MS and then receives data, divides it by 1.2 (MEASURE_RATIO) and saves it
     * inside of light_intensity.
     *
     * @param[out] light_intensity Reference for storing the final illuminance value in lux
     * @return ESP_OK if measurement succeed.
     * @return Other esp errors if something went wrong.
     */
    esp_err_t read_light_intensity_blocking(uint32_t &light_intensity);

    /**
    * @brief Triggers light intensity measurement.
    *
    * Sends order to sensor via I2C protocol to start measure procedure.
    *
    * @return ESP_OK: I2C master transmit success.
    * @return ESP_ERR_INVALID_RESPONSE: I2C master transmit receives NACK.
    * @return ESP_ERR_INVALID_ARG: I2C master transmit parameter invalid.
    * @return ESP_ERR_TIMEOUT: Operation timeout(larger than xfer_timeout_ms) because the bus is busy or hardware crash
    */
    esp_err_t trigger_measurement();

    /**
     * @brief Read measurement data from sensor.
     *
     * Fetches data from sensor. Divides it by MEASURE_RATIO and then casts it to uint32_t. Finally saves to light_intensity reference.
     *
     * @param[out] light_intensity Reference for storing the final illuminance value in lux
     * @return ESP_OK: I2C master receive success
     * @return ESP_ERR_INVALID_ARG: I2C master receive parameter invalid.
     * @return ESP_ERR_TIMEOUT: Operation timeout(larger than xfer_timeout_ms) because the bus is busy or hardware crash
     */
    esp_err_t fetch_measurement(uint32_t &light_intensity);
};

#endif //FIRMWARE_BH1750_H
