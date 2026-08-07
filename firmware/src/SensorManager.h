//
// Created by Wiktor on 05.08.2026.
//

#ifndef FIRMWARE_SENSORMANAGER_H
#define FIRMWARE_SENSORMANAGER_H
#include "BH1750.h"
#include "BME280.h"

class SensorManager {
private:
    BME280 bme280;
    BH1750 bh1750;

    public:
        struct Payload {
            uint32_t light_intensity; // TODO: change to uint?
            int32_t temperature;
            uint32_t humidity;
            uint32_t pressure;
        };

        SensorManager(i2c_master_bus_handle_t i2c_master_bus_handle);
        Payload getPayload();

};

#endif //FIRMWARE_SENSORMANAGER_H