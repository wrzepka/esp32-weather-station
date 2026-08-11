//
// Created by Wiktor on 11.08.2026.
//

#ifndef FIRMWARE_IPAYLOADSERIALIZER_H
#define FIRMWARE_IPAYLOADSERIALIZER_H
#include <esp_err.h>
#include <string>

#include "SensorManager.h"

class IPayloadSerializer {
public:
    virtual ~IPayloadSerializer() = default;

    virtual std::string serialize(const SensorManager::Payload&) = 0;
};

#endif //FIRMWARE_IPAYLOADSERIALIZER_H