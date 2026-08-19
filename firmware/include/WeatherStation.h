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
     * @brief Initializes sensors via SensorManager.
     *
     * @return ESP_OK if initialization succeed.
     * @return Many others ESP errors if something went wrong
     */
    esp_err_t init_sensors();

    /**
     * @brief Establishes communication.
     *
     * Initializes Wi-Fi via WiFiModule, then connects with the data transport broker e.g. MQTT.
     *
     * @return ESP_OK if initialization succeed.
     * @return Other esp errors if something went wrong
     */
    esp_err_t init_communication();

    /**
     * @brief Takes measures and fills up Payload structure.
     *
     *
     * @return ESP_OK if fetches data from at least one sensor.
     * @return ESP_FAIL if all sensors are down.
     */
    esp_err_t measure();

    /**
     * @brief Sends payload via defined transport protocol.
     *
     * Serializes current data Payload, then initializes communication via init_communication() and uploads data via
     * defined transport protocol.
     *
     * @return ESP_OK if initialization succeed.
     * @return Other esp errors if something went wrong
     */
    esp_err_t send();

    /**
    * @brief Stops communication and then goes into deep sleep.
    *
    * @note This method will be modified/expanded so for now, documentation will be as simple as should be.
    *
    * @return ESP_OK if initialization succeed.
    * @return Other esp errors if something went wrong
    */
    esp_err_t sleep();

};

#endif //FIRMWARE_WEATHERSTATION_H