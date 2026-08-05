//
// Created by Wiktor on 24.07.2026.
//

#ifndef FIRMWARE_MQTTTRANSPORT_H
#define FIRMWARE_MQTTTRANSPORT_H
#include <esp_event_base.h>
#include "mqtt_client.h"
#include "ITelemetryTransport.h"

/**
 * @class MqttTransport
 * @brief Class for handling message publishing via MQTT protocol.
 *
 * This class provides configuration and connection initialization with message publishing, using MQTT protocol.
 */
class MqttTransport : public ITelemetryTransport {
private:
    const char *m_brokerIp;/**<Mqtt broker IP address.*/
    uint16_t m_brokerPort;/**< Mqtt broker port (default: 1833).*/
    esp_mqtt_client_handle_t m_client;/**< Mqtt client handle.*/
    EventGroupHandle_t m_event_group_handle;/**< EventGroup handle.*/
    static constexpr uint8_t MQTT_DISCONNECTED_BIT = BIT0;/**<Bit standing for disconnection.*/
    static constexpr uint8_t MQTT_PUBLISHED_BIT = BIT1;/**<Bit standing for successful message publication.*/
    static constexpr uint8_t MQTT_ERROR_BIT = BIT2;/**<Bit standing for some error encounter.*/
    static constexpr uint32_t MAX_EVENT_GROUP_WAIT_TIME = 5000;/**< Max waiting time for response from EventGroup functions.*/
    static constexpr auto TOPIC = "iot/weather";/**<Topic used for message recognition.*/
    static constexpr int QOS = 1;/**<Level of message's Quality Of Service.*/

    /**
     * @brief Event handler for version 5 mqtt protocol.
     *
     * This handler provides handling of the built-in MQTT_EVENT_X events. It also provies basic logging via ESP_LOGI function.
     *
     * @param handler_args Pointer to context. In this implementation here is stored object of MqttTransport class.
     * @param base Event namespace category.
     * @param event_id Category unique event identifier.
     * @param event_data Dynamic payload, based on event_id.
     */
    static void mqtt5_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

public:
    MqttTransport(const char *brokerIp, uint16_t brokerPort) : m_brokerIp(brokerIp), m_brokerPort(brokerPort), m_client(nullptr), m_event_group_handle(xEventGroupCreate()) {
    };

    ~MqttTransport() {
        if (this->m_client != nullptr) {
            esp_mqtt_client_stop(this->m_client);
            esp_mqtt_client_destroy(this->m_client);
            this->m_client = nullptr;
        }
        if (this->m_event_group_handle != nullptr) {
            vEventGroupDelete(this->m_event_group_handle);
            this->m_event_group_handle = nullptr;
        }
    }

    /**
     * @note This is mandatory implementation of ITelemetryTransport interface method.
     * @brief Initializes, configures, starts MQTT service.
     *
     * Creates handles, configures service for using version 5 of MQTT. Starts service, and then it is waiting for
     * successful message publishing by utilizing Event Bits.
     *
     * @return ESP_OK if initialization succeed.
     * @return ESP_FAIL if something went wrong.
     */
    esp_err_t connect() override;

    /**
     * @note This is mandatory implementation of ITelemetryTransport interface method.
     * @brief Disconnects station from the broker.
     *
     * @return ESP_OK if initialization succeed.
     * @return ESP_ERR_INVALID_ARG if something went wrong.
     */
    esp_err_t disconnect() override;

    /**
     * @note This is mandatory implementation of ITelemetryTransport interface method.
     * @brief Publish message to the broker.
     *
     * @return ESP_OK if message publishing succeed.
     * @return ESP_FAIL if something went wrong.
     */
    esp_err_t publish() override;
};


#endif //FIRMWARE_MQTTTRANSPORT_H
