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
    static constexpr uint8_t DEFAULT_I2C_ADDRESS = 0x20; /**< Default I2C address for BH1750 sensor.*/
    static constexpr uint8_t SECONDARY_I2C_ADDRESS = 0x5c; /**< Secondary I2C address for BH1750 sensor if ADDR is HIGH.*/
private:
    uint8_t m_address; /**< I2C device address*/
    i2c_master_dev_handle_t m_dev_handle; /**< I2C device handle*/
    static constexpr uint8_t ONE_TIME_H_RESOLUTION_MODE = 0x20; /**< Value for high resolution one time measurement*/
    static constexpr float MEASURE_RATIO = 1.2; /** Ratio that is used for calculating proper illuminance. Value is from docs.*/
    static constexpr uint8_t SUITABLE_MEASUREMENT_DELAY_IN_MS = 180; /**< Appropriate delay for proper measurement. */
    static constexpr uint8_t MAX_RESPONSE_TIME_IN_MS = 100; /** Maximum waiting time for response in ms.*/
public:
    BH1750(const uint8_t address = DEFAULT_I2C_ADDRESS) : m_address(address), m_dev_handle(nullptr) {
    }

    /**
     * @brief Initializes I2C connection for BH1750 sensor.
     *
     * Configures device handler and adds it to the I2C bus.
     *
     * @param bus_handle I2C master bus handle.
     * @return false when initialization failed, otherwise true.
     */
    bool begin(i2c_master_bus_handle_t bus_handle);

    /**
     * @brief Reads light intensity and returns it
     *
     * Starts one-time measurement, waits 180ms and reads raw value. After adding bytes and dividing by 1.2 returns proper
     * value.
     *
     * @return Unsigned integer illuminance value in lux (lx).
     */
    uint32_t read_light_intensity();
};

#endif //FIRMWARE_BH1750_H
