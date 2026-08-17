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
public:
    /**
     * @brief Serializes payload to JSON format.
     *
     * This method serialize payload data into stringified JSON format using cJSON component.
     *
     * @param payload reference to Payload structure.
     * @param[out] serialize_string reference to serialized string in json format, used further in data transportation.
     * @return ESP_OK if serialization succeed.
     * @return ESP_ERR_NO_MEM if there is not enough memory for allocating new cJSON object or raw string.
     */
    esp_err_t serialize(const SensorManager::Payload &payload, std::string &serialize_string) override;
};
