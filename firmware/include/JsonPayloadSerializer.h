/**
 * @file JsonPayloadSerializer.h
 * @author Wiktor
 * @brief Class used for serializing payload to JSON format.
 * @date 11.08.2026
 **/
#ifndef FIRMWARE_JSONPAYLOADSERIALIZER_H
#define FIRMWARE_JSONPAYLOADSERIALIZER_H
#include "IPayloadSerializer.h"

#endif //FIRMWARE_JSONPAYLOADSERIALIZER_H

/**
 * @class JsonPayloadSerializer
 * @brief Class used for serializing weather station payload to JSON format.
 *
 * This class provides way o serializing payload to JSON format.
 */
class JsonPayloadSerializer : public IPayloadSerializer {
    /**
     * @brief Serializes payload to JSON format.
     *
     * This method serialize payload data into stringified JSON format using cJSON component.
     *
     * @param payload reference to Payload structure.
     * @return Payload serialized to string in JSON format.
     */
public:
    std::string serialize(const SensorManager::Payload &payload) override;
};
