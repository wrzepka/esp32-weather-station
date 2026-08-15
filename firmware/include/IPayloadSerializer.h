/**
 * @file IPayloadSerializer.h
 * @author Wiktor
 * @brief Interface for declaring methods for serializing weather data payload.
 * @date 11.08.2026
 **/
#ifndef FIRMWARE_IPAYLOADSERIALIZER_H
#define FIRMWARE_IPAYLOADSERIALIZER_H
#include <esp_err.h>
#include <string>

#include "SensorManager.h"

/**
 * @class IPayloadSerializer
 * @brief Interface for implementing way of serializing weather station telemetry data.
 *
 * This interface provides a method that must be implemented for some type of serializer for example in JSON format.
 */
class IPayloadSerializer {
public:
    virtual ~IPayloadSerializer() = default;

    /**
     * @brief Serializes payload into a target representation.
     *
     * @param payload reference to Payload structure.
     * @return Payload serialized to string.
     */
    virtual std::string serialize(const SensorManager::Payload& payload) = 0;
};

#endif //FIRMWARE_IPAYLOADSERIALIZER_H