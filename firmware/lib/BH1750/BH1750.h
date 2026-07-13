//
// Created by Wiktor on 12.07.2026.
//

#ifndef FIRMWARE_BH1750_H
#define FIRMWARE_BH1750_H
#include <driver/i2c_types.h>

class BH1750 {
private:
    uint8_t _address;
    i2c_master_dev_handle_t _dev_handle;
    static constexpr uint8_t ONE_TIME_H_RESOLUTION_MODE = 0x20;

public:
    BH1750(const uint8_t address = 0x23) : _address(address), _dev_handle(nullptr) {}

    bool begin(i2c_master_bus_handle_t bus_handle);
    uint32_t read_light_intensity() const;
};

#endif //FIRMWARE_BH1750_H
