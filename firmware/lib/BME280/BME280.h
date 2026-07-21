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
    static constexpr uint8_t DEFAULT_I2C_ADDR = 0x76;
    static constexpr uint8_t SECONDARY_I2C_ADDR = 0x77;

    /**
     * @brief Contains BME280 calibration data for temperature, pressure and humidity.
     */
    struct bme280_calib_data {
        uint16_t dig_T1; /**<calibration T1 data*/
        int16_t dig_T2; /**<calibration T2 data*/
        int16_t dig_T3; /**<calibration T3 data*/

        uint16_t dig_P1; /**<calibration P1 data*/
        int16_t dig_P2; /**<calibration P2 data*/
        int16_t dig_P3; /**<calibration P3 data*/
        int16_t dig_P4; /**<calibration P4 data*/
        int16_t dig_P5; /**<calibration P5 data*/
        int16_t dig_P6; /**<calibration P6 data*/
        int16_t dig_P7; /**<calibration P7 data*/
        int16_t dig_P8; /**<calibration P8 data*/
        int16_t dig_P9; /**<calibration P9 data*/

        uint8_t dig_H1; /**<calibration H1 data*/
        int16_t dig_H2; /**<calibration H2 data*/
        uint8_t dig_H3; /**<calibration H3 data*/
        int16_t dig_H4; /**<calibration H4 data*/
        int16_t dig_H5; /**<calibration H5 data*/
        int8_t dig_H6; /**<calibration H6 data*/

        int32_t t_fine; /**<calibration T_FINE data*/
    };

    //TODO: find alternative (hex i guess)
    union bme_280_ctrl_meas {
        struct {
            uint8_t mode: 2;
            uint8_t osrs_p: 3;
            uint8_t osrs_t: 3;
        } fields;

        uint8_t raw;
    };

    union bme_280_hum_meas {
        struct {
            uint8_t osrs_h: 3;
            uint8_t RESERVED: 5;
        } fields;

        uint8_t raw;
    };

    BME280(uint8_t address = DEFAULT_I2C_ADDR) : _address(address), _dev_handle(nullptr), _calib_data() {
    };

    /**
     * @brief Initializes I2C connection for BME280 sensor.
     *
     * Configures device handler, add it to I2C bus. Next set up ctrl_hum and ctrl_meas registers with sleep mode and x1
     * oversampling.
     *
     * @param bus_handle I2C master bus handle.
     * @return false when initialization failed, otherwise true.
     */
    esp_err_t begin(i2c_master_bus_handle_t bus_handle);

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
     * @return static reference to _calib_data field.
     */
    const bme280_calib_data &calib_data() const;

    /**
     * @brief
     */
    void print_calib_data() const;

    /**
     * @brief Compensates ADC temperature value and returns it in integer format.
     *
     * Calculates compensated value based on 32bit formula from datasheet.
     *
     * @param adc_temp ADC temperature value read from sensor registers.
     * @return Compensated value in integer format (ex. 2560 - 25,60*C)
     */
    int32_t compensate_temperature(int32_t adc_temp);

    /**
     * @brief Compensates ADC pressure value and returns it in integer format.
     *
     * Calculates compensated value based on 32bit formula from datasheet.
     *
     * @param adc_press ADC pressure value read from sensor registers.
     * @return Compensated value in unsigned integer format (ex. 96368 - 963.68hPa)
     */
    uint32_t compensate_pressure(int32_t adc_press);

    /**
     * @brief Compensates ADC humidity value and returns it in integer format.
     *
     * Calculates compensated value based on 32bit formula from datasheet.
     *
     * @param adc_H ADC humidity value read from sensor registers.
     * @return Compensated value in unsigned integer format (ex. 47445 - 46.333 %RH (after dividing by 1024!))
     */
    uint32_t compensate_humidity(int32_t adc_H);

    /**
     * @brief Reads ADC values for temperature, pressure and humidity. Compensate them and temporarily LOGs them.
     *
     * Firstly enables BME280 sensor to Force Mode and waits ~10ms to write up conf. Next reads ADC values and
     * compensates them. At the end LOGs them using ESP_LOGI function.
     *
     * @return false when read failed, otherwise true.
     */
    esp_err_t read_weather_data();

private:
    uint8_t _address; /**< Device I2C address (default: 0x76 or 0x77 if specified pin is high) */
    i2c_master_dev_handle_t _dev_handle; /**< I2C device handle */
    bme280_calib_data _calib_data; /**< Structure for calibration data*/
    int32_t fine_temp; /**< Value from compensate_temperature() used in other compensational methods*/
    static constexpr uint8_t REG_CALIB_00_ADDR = 0x88;
    static constexpr uint8_t REG_CALIB_26_ADDR = 0xE1;
    static constexpr uint8_t REG_CONTROL_HUM_ADDR = 0xF2;
    static constexpr uint8_t REG_CONTROL_MEAS_ADDR = 0xF4;
    static constexpr uint8_t REG_MEAS_DATA_START_ADDR = 0xF7;
    static constexpr uint8_t CALIB_00_PAYLOAD_SIZE = 26;
    static constexpr uint8_t CALIB_26_PAYLOAD_SIZE = 7;
    static constexpr uint8_t MEAS_DATA_PAYLOAD_SIZE = 8;
    static constexpr uint32_t MAX_RESPONSE_TIME_IN_MS = 100;
    static constexpr uint32_t SUITABLE_MEASUREMENT_DELAY_IN_MS = 100;
};

#endif //FIRMWARE_BME280_H
