/**
 * @file WeatherStation.h
 * @author Wiktor
 * @brief Class for managing whole app logic such as: sensors, wifi, data transport and deep sleep.
 * @date 18.08.2026
 **/
#ifndef FIRMWARE_WEATHERSTATION_H
#define FIRMWARE_WEATHERSTATION_H
#include <esp_err.h>
#include <driver/i2c_types.h>

#include "MqttTransport.h"
#include "SensorManager.h"
#include "WiFiManager.h"

/**
 * @class WeatherStation
 * @brief Main class for handling whole application logic.
 *
 * Handles sensors, Wi-Fi, data transport and deep sleep.
 */
class WeatherStation {
    SensorManager::Payload current_payload{};/**<Current weather data payload.*/
    SensorManager sensor_manager;/**Instance of sensor manager.*/
    WiFiManager wifi_manager;/**Instance of Wi-Fi manager.*/
    MqttTransport mqtt_transport;/**Instance of Wi-Fi manager.*/ //Change to interface ITelemtryTransport?

public:
    WeatherStation(i2c_master_bus_handle_t i2c_bus_handle, const char* brokerIp, uint16_t brokerPort): sensor_manager(i2c_bus_handle), mqtt_transport(brokerIp, brokerPort){};

    /**
     * TODO: docs after methods polishing.
     * @return
     */
    esp_err_t init_sensors();

    /**
     * TODO: docs after methods polishing.
     * @return
     */
    esp_err_t init_communication();

    /**
     * TODO: docs after methods polishing.
     * @return
     */
    esp_err_t measure();

    /**
     * TODO: docs after methods polishing.
     * @return
     */
    esp_err_t send();

    /**
    * TODO: docs after methods polishing.
    * @return
    */
    esp_err_t sleep();

};

#endif //FIRMWARE_WEATHERSTATION_H