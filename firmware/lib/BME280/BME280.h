/**
 * @file BME280.h
 * @author Wiktor
 * @brief Driver library for BME280 sensor using ESP-IDF I2C master driver.
 * @date 2026-07-13
 *
 * @note Compensation algorithms and data structures are adapted from the official Bosch Sensortec
 *       BME280 reference implementation (BSD-3-Clause). See
 *       /firmware/THIRD_PARTY_LICENSES/BME280_BSD3.txt for full license text.
 *
 * @see https://github.com/boschsensortec/BME280_driver
 */

#ifndef FIRMWARE_BME280_H
#define FIRMWARE_BME280_H
#include <esp_err.h>
#include <driver/i2c_types.h>

/**
 * @class BME280
 * @brief Class for interaction with BME280 temperature, pressure and humidity sensor.
 *
 * This class provides for now basic control over BME280 sensor using ESP_IDF I2C driver.
 * It handles sensor initialization and configuration, calibration data readout and methods for raw ADC values
 * compensation.
 *
 */
class BME280 {
public:
    static constexpr uint8_t DEFAULT_I2C_ADDR = 0x76 /**< Default I2C address for BME280 sensor.*/;
    static constexpr uint8_t SECONDARY_I2C_ADDR = 0x77; /**< Secondary I2C address for BME280 sensor.*/;

    /**
     * @brief Contains BME280 calibration data for temperature, pressure and humidity.
     */
    struct bme280_calib_data {
        uint16_t dig_T1; /**<calibration T1 data.*/
        int16_t dig_T2; /**<calibration T2 data.*/
        int16_t dig_T3; /**<calibration T3 data.*/

        uint16_t dig_P1; /**<calibration P1 data.*/
        int16_t dig_P2; /**<calibration P2 data.*/
        int16_t dig_P3; /**<calibration P3 data.*/
        int16_t dig_P4; /**<calibration P4 data.*/
        int16_t dig_P5; /**<calibration P5 data.*/
        int16_t dig_P6; /**<calibration P6 data.*/
        int16_t dig_P7; /**<calibration P7 data.*/
        int16_t dig_P8; /**<calibration P8 data.*/
        int16_t dig_P9; /**<calibration P9 data.*/

        uint8_t dig_H1; /**<calibration H1 data.*/
        int16_t dig_H2; /**<calibration H2 data.*/
        uint8_t dig_H3; /**<calibration H3 data.*/
        int16_t dig_H4; /**<calibration H4 data.*/
        int16_t dig_H5; /**<calibration H5 data.*/
        int8_t dig_H6; /**<calibration H6 data.*/

        int32_t t_fine; /**<calibration T_FINE data.*/
    };

    /**
     * @brief BME280 oversampling setting.
     */
    enum class Oversampling : uint8_t {
        Skipped = 0x00,
        X1 = 0x01,
        X2 = 0x02,
        X4 = 0x03,
        X8 = 0x04,
        X16 = 0x05,
    };

    /**
     * @brief BME280 mode of work.
     */
    enum class Mode : uint8_t {
        Sleep = 0x00,
        Forced = 0x01,
        Normal = 0x03
    };

    /**
     * @brief Config structure for easier ctrl_hum and ctrl_meas register configuration.
     */
    struct Config {
        Oversampling temp_oversampling = Oversampling::X1;
        Oversampling press_oversampling = Oversampling::X1;
        Oversampling hum_oversampling = Oversampling::X1;
        Mode mode = Mode::Sleep;
    };

    /**
     * @brief Data structure for clear data acquisition.
     */
    struct Data { //TODO: change datatypes? these are too big imo.
        int32_t temperature;
        uint32_t humidity;
        uint32_t pressure;
    };

    BME280(uint8_t address = DEFAULT_I2C_ADDR) : _address(address), _dev_handle(nullptr), _calib_data() {
    };

    /**
     * @brief Initializes I2C connection for BME280 sensor.
     *
     * Configures device handler, add it to I2C bus. Next initializes apply_config() and read_calib_data().
     *
     * @param bus_handle I2C master bus handle.
     * @param config BME280 configuration structure.
     * @return ESP_ERR_INVALID_ARG if bus handle is nullptr.
     * @return ESP_OK if initialization succeed.
     * @return Other esp errors if something went wrong.
     */
    esp_err_t begin(i2c_master_bus_handle_t bus_handle, const Config &config);

    /**
     * @brief Apply Config structure.
     *
     * Uses Config structure to setups ctrl_hum and ctrl_meas registers.
     *
     * @param config BME280 configuration structure.
     * @return ESP_OK if configuration setup succeed.
     * @return Other esp errors if something went wrong.
     */
    esp_err_t apply_config(const Config &config);

    /**
     * @brief Reads calibration data from BME280 registers.
     *
     * Reads data from specified registers, then puts it inside bme280_calib_data structure (_calib_data field).
     *
     * @return false when read failed, otherwise true.
     */
    esp_err_t read_calib_data();

    /**
     * @brief Gives access to private _calib_data field.
    * @return ESP_OK if configuration setup succeed.
     * @return Other esp errors if something went wrong.
     */
    const bme280_calib_data &calib_data() const;

    /**
     * @brief Prints calibration parameters.
     */
    void print_calib_data() const;

    /**
     * @brief Compensates ADC temperature value and returns it in integer format.
     *
     * Calculates compensated value based on 32bit formula from datasheet.
     *
     * @param adc_temp ADC temperature value read from sensor registers.
     * @return Compensated value in integer format (ex. 2560 - 25,60*C).
     */
    int32_t compensate_temperature(int32_t adc_temp);

    /**
     * @brief Compensates ADC pressure value and returns it in integer format.
     *
     * Calculates compensated value based on 32bit formula from datasheet.
     *
     * @param adc_press ADC pressure value read from sensor registers.
     * @return Compensated value in unsigned integer format (ex. 96368 - 963.68hPa).
     */
    uint32_t compensate_pressure(int32_t adc_press);

    /**
     * @brief Compensates ADC humidity value and returns it in integer format.
     *
     * Calculates compensated value based on 32bit formula from datasheet.
     *
     * @param adc_H ADC humidity value read from sensor registers.
     * @return Compensated value in unsigned integer format (ex. 47445 - 46.333 %RH (after dividing by 1024!)).
     */
    uint32_t compensate_humidity(int32_t adc_H);

    /**
     * @note Blocking method.
     * @brief Reads ADC values for temperature, pressure and humidity. Compensate them and temporarily LOGs them.
     *
     * Firstly enables BME280 sensor to Force Mode and waits ~10ms to write up conf. Next reads ADC values and
     * compensates them. At the end LOGs them using ESP_LOGI function.
     *
     * @param[out] data Reference of Data structure, used for storing measurement data.
     * @return ESP_OK: I2C master transmit success.
     * @return ESP_ERR_INVALID_RESPONSE: I2C master transmit receives NACK.
     * @return ESP_ERR_INVALID_ARG: I2C master transmit parameter invalid.
     * @return ESP_ERR_TIMEOUT: Operation timeout(larger than xfer_timeout_ms) because the bus is busy or hardware crash.
     */
    esp_err_t read_sensor_data_blocking(Data& data);

    /**
     * @brief Orders sensor to start measurements.
     *
     * @return ESP_OK: I2C master transmit success.
     * @return ESP_ERR_INVALID_RESPONSE: I2C master transmit receives NACK.
     * @return ESP_ERR_INVALID_ARG: I2C master transmit parameter invalid.
     * @return ESP_ERR_TIMEOUT: Operation timeout(larger than xfer_timeout_ms) because the bus is busy or hardware crash.
     */
    esp_err_t trigger_measurement();

    /**
     * @brief Fetches data from sensor registers.
     *
     * Reads data, compensates them and saves it via data reference.
     *
     * @param[out] data Reference of Data structure, used for storing measurement data.
     * @return ESP_OK: I2C master transmit success.
     * @return ESP_ERR_INVALID_RESPONSE: I2C master transmit receives NACK.
     * @return ESP_ERR_INVALID_ARG: I2C master transmit parameter invalid.
     * @return ESP_ERR_TIMEOUT: Operation timeout(larger than xfer_timeout_ms) because the bus is busy or hardware crash.
     */
    esp_err_t fetch_measurement(Data& data);
private:
    uint8_t _address; /**< Device I2C address (default: 0x76 or 0x77 if specified pin is high).*/
    i2c_master_dev_handle_t _dev_handle; /**< I2C device handle.*/
    bme280_calib_data _calib_data; /**< Structure for calibration data.*/
    int32_t fine_temp; /**< Value from compensate_temperature() used in other compensational methods.*/
    static constexpr uint8_t REG_CALIB_00_ADDR = 0x88; /** Address of the first part of calib data register.*/
    static constexpr uint8_t REG_CALIB_26_ADDR = 0xE1; /** Address of the second part of calib data register.*/
    static constexpr uint8_t REG_CONTROL_HUM_ADDR = 0xF2; /** ctrl_hum register address.*/
    static constexpr uint8_t REG_CONTROL_MEAS_ADDR = 0xF4; /** ctrl_meas register address.*/
    static constexpr uint8_t REG_MEAS_DATA_START_ADDR = 0xF7; /** Address of the start of the measurement data payload.*/
    static constexpr uint8_t CALIB_00_PAYLOAD_SIZE = 26; /** Size of the first part of calib data payload.*/
    static constexpr uint8_t CALIB_26_PAYLOAD_SIZE = 7; /** Size of the second part of calib data payload.*/
    static constexpr uint8_t MEAS_DATA_PAYLOAD_SIZE = 8; /** Size of the measurement data payload.*/
    static constexpr uint32_t MAX_RESPONSE_TIME_IN_MS = 100; /** Maximum waiting time for response in ms.*/
    static constexpr uint32_t SUITABLE_MEASUREMENT_DELAY_IN_MS = 50; /** Appropriate delay for proper measurement.*/
};

#endif //FIRMWARE_BME280_H
