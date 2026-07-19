/**
 * @file WiFiManager.h
 * @author Wiktor
 * @brief Class for handling connection with Wi-Fi WPA2
 * @date 17.07.2026
 *
 * @note Implementation is based on official ESP-IDF examples:
 * @see https://github.com/espressif/esp-idf/tree/v6.0.2/examples/protocols/static_ip
 **/
#ifndef FIRMWARE_WIFIMANAGER_H
#define FIRMWARE_WIFIMANAGER_H
#include "esp_wifi.h"

/**
 * @class WiFiManager
 * @brief Class for Wi-Fi connection handling.
 *
 * This class provides configuration and initialization of ESP32 Wi-Fi module.
 *
 * @note In near future it will have also static ip setup and deep sleep integration.
 */
class WiFiManager {
    private:
    esp_netif_t *_sta_netif; /**< Pointer to station network interface handle*/
    esp_event_handler_instance_t _instance_any_id; /**< Handle for WiFi types events*/
    esp_event_handler_instance_t _instance_got_ip; /**< Handle for IP types events*/
    EventGroupHandle_t _s_network_event_group; /**< Handle for network event group*/

    public:
    WiFiManager(): _sta_netif(nullptr), _instance_any_id(nullptr), _instance_got_ip(nullptr), _s_network_event_group(nullptr) {};

    /**
     * @brief Callback function for Wi-Fi events.
     *
     * Based on event_it, it is handling connection to AP, reconnection, static ip setup or communicating
     * that connection is established.
     *
     * @param arg Pointer to context. In this implementation here is stored object of WiFiManager class.
     * @param event_base Event namespace category ex. WIFI_EVENT, IP_EVENT.
     * @param event_id Category unique event identifier.
     * @param event_data Dynamic payload, based on even_id.
     */
    static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                   int32_t event_id, void* event_data);

    /**
     * @brief Initializes and configures Wi-Fi module.
     *
     * Creates and uses handles for network interface, events and event group. Next setups wi-fi module
     * for work in STATION MODE.
     */
    void init_wifi_station();

    void set_static_ip();
};


#endif //FIRMWARE_WIFIMANAGER_H