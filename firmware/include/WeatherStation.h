//
// Created by Wiktor on 13.08.2026.
//

#ifndef FIRMWARE_WEATHERSTATION_H
#define FIRMWARE_WEATHERSTATION_H
#include <esp_err.h>
#include <driver/i2c_types.h>

#include "MqttTransport.h"
#include "SensorManager.h"
#include "WiFiManager.h"

//TODO: docs
class WeatherStation {
    SensorManager::Payload current_payload{};
    SensorManager sensor_manager;
    WiFiManager wifi_manager;
    MqttTransport mqtt_transport;

public:
    WeatherStation(i2c_master_bus_handle_t i2c_bus_handle, const char* brokerIp, uint16_t brokerPort): sensor_manager(i2c_bus_handle), mqtt_transport(brokerIp, brokerPort){};

    esp_err_t init_sensors();
    esp_err_t init_communication();
    esp_err_t measure();
    esp_err_t send();
    esp_err_t sleep();

};

#endif //FIRMWARE_WEATHERSTATION_H