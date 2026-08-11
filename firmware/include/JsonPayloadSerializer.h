//
// Created by Wiktor on 11.08.2026.
//

#ifndef FIRMWARE_JSONPAYLOADSERIALIZER_H
#define FIRMWARE_JSONPAYLOADSERIALIZER_H
#include "IPayloadSerializer.h"

#endif //FIRMWARE_JSONPAYLOADSERIALIZER_H

class JsonPayloadSerializer: public IPayloadSerializer {
    std::string serialize(const SensorManager::Payload& payload) override;
};