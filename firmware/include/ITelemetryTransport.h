/**
 * @file ITelemetryTransport.h
 * @author Wiktor
 * @brief Interface for declaring methods for transport protocols ex. MQTT.
 * @date 24.07.2026
 **/
#ifndef FIRMWARE_ITELEMETRYTRANSPORT_H
#define FIRMWARE_ITELEMETRYTRANSPORT_H
#include <esp_err.h>
#include <string>

/**
 * @class ITelemetryTransport
 * @brief Interface for implementing way of transporting weather station telemetry data
 *
 * This interface provides methods that must be implemented for some type of transport protocol.
 */
class ITelemetryTransport {
public:
    virtual ~ITelemetryTransport() = default;

    /**
     * @brief Establishes connection with remote server/broker.
     *
     * @return ESP_OK on success, proper esp_err_t otherwise.
     */
    virtual esp_err_t connect() = 0;

    /**
     * @brief Closes the connection.
     *
     * @return ESP_OK on success, proper esp_err_t otherwise.
     */
    virtual esp_err_t disconnect() = 0;

    /**
     * @brief Publishes payload to the destination.
     *
     * @param serializedPayload serialized payload to the string.
     * @return ESP_OK on success, proper esp_err_t otherwise.
     */
    virtual esp_err_t publish(const std::string& serializedPayload) = 0;
};

#endif //FIRMWARE_ITELEMETRYTRANSPORT_H
