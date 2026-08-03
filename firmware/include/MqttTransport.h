//
// Created by Wiktor on 24.07.2026.
//

#ifndef FIRMWARE_MQTTTRANSPORT_H
#define FIRMWARE_MQTTTRANSPORT_H
#include <esp_event_base.h>
#include "mqtt_client.h"
#include "ITelemetryTransport.h"

/**
 * TODO
 */
class MqttTransport : public ITelemetryTransport {
private:
    const char *m_brokerIp;/**<Mqtt broker IP address*/
    uint16_t m_brokerPort;/**< Mqtt broker port (default: 1833)*/
    esp_mqtt_client_handle_t m_client;/**< Mqtt client handle*/
    EventGroupHandle_t m_even_group_handle;
    /**
     * @brief TODO
     * @param handler_args
     * @param base
     * @param event_id
     * @param event_data
     */
    static void mqtt5_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

public:
    MqttTransport(const char *brokerIp, uint16_t brokerPort) : m_brokerIp(brokerIp), m_brokerPort(brokerPort), m_client(nullptr), m_even_group_handle(xEventGroupCreate()) {
    };

    ~MqttTransport() {
        if (this->m_client != nullptr) {
            esp_mqtt_client_destroy(this->m_client);
        }
        if (this->m_even_group_handle != nullptr) {
            vEventGroupDelete(this->m_even_group_handle);
        }
    }

    /**
     * TODO
     * @return
     */
    esp_err_t connect() override;

    /**
     * TODO
     * @return
     */
    esp_err_t disconnect() override;

    /**
     * TODO
     * @return
     */
    esp_err_t publish() override;
};


#endif //FIRMWARE_MQTTTRANSPORT_H
