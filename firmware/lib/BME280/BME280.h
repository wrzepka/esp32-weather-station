//
// Created by Wiktor on 13.07.2026.
//

#ifndef FIRMWARE_BME280_H
#define FIRMWARE_BME280_H
#include <driver/i2c_types.h>

class BME280 {

public:
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
            uint8_t mode : 2;
            uint8_t osrs_p : 3;
            uint8_t osrs_t : 3;
        } fields;
        uint8_t raw;
    };

    union bme_280_hum_meas {
        struct {
            uint8_t osrs_h : 3;
            uint8_t RESERVED : 5;
        } fields;
        uint8_t raw;
    };

    BME280(uint8_t address = 0x76) : _address(address), _dev_handle(nullptr), _calib_data() {
    };

    bool begin(i2c_master_bus_handle_t bus_handle);
    bool read_calib_data();

    const bme280_calib_data& calib_data() const;
    void print_calib_data() const;
    int32_t compensate_temperature(int32_t adc_temp);
    uint32_t compensate_pressure(int32_t adc_press);
    uint32_t compensate_humidity(int32_t adc_H);
    bool read_weather_data();
private:
    uint8_t _address;
    i2c_master_dev_handle_t _dev_handle;
    bme280_calib_data _calib_data;
    int32_t fine_temp;
};

#endif //FIRMWARE_BME280_H
