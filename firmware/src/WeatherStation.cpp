//
// Created by Wiktor on 13.08.2026.
//

#include "WeatherStation.h"

#include "JsonPayloadSerializer.h"
// TODO: better error logging? inside of SensorManager?
esp_err_t WeatherStation::init_sensors() {
    return sensor_manager.init_sensors();
};

esp_err_t WeatherStation::init_communication() {
    esp_err_t result = wifi_manager.init_wifi_station();
    if (result != ESP_OK) {
        return result;
    }

    return mqtt_transport.connect();
};

esp_err_t WeatherStation::measure() {
    return sensor_manager.getPayload(current_payload);
}

esp_err_t WeatherStation::send() {
    JsonPayloadSerializer payload_serializer = JsonPayloadSerializer();

    //Here will be some sort of error checking. If serialization succeed :D
    std::string serialized_string = payload_serializer.serialize(current_payload);

    esp_err_t init_communication_result = init_communication();
    if (init_communication_result != ESP_OK) {
        return init_communication_result;
    }

    return mqtt_transport.publish(serialized_string);
}

esp_err_t WeatherStation::sleep() {
    //TODO: method
    return ESP_OK;
}

