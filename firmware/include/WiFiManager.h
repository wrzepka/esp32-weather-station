/**
 * @file WiFiManager.h
 * @author Wiktor
 * @brief Class for handling connection with Wi-Fi WPA2
 * @date 18.08.2026
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
    esp_netif_t *_sta_netif; /**< Pointer to station network interface handle.*/
    esp_event_handler_instance_t _instance_any_id; /**< Handle for Wi-Fi types events.*/
    esp_event_handler_instance_t _instance_got_ip; /**< Handle for IP types events.*/
    static constexpr auto WIFI_SSID = "PocoF5"; /**< SSID of used Wi-Fi network.*/
    static constexpr auto WIFI_PASSWORD = "12345678"; /**< Password of used Wi-Fi network.*/
    static constexpr uint32_t MAX_EVENT_GROUP_WAIT_TIME = 5000 /**< Max waiting time for response from EventGroup functions.*/;
    static constexpr uint8_t WIFI_CONNECTED_BIT = BIT0; /**< BIT standing for successful connection*/
    static constexpr uint8_t WIFI_DISCONNECTED_BIT = BIT1; /**< BIT standing for disconnection*/
    static constexpr uint8_t MAX_RETRY = 5; /** Amount of Wi-Fi reconnect tries*/
    static constexpr auto STATION_IP = "10.246.161.67"; /** Local IP address of this station*/
    static constexpr auto STATION_NETMASK = "255.255.255.0"; /** Network mask of this station's network*/
    static constexpr auto STATION_GATEWAY = "10.246.161.1"; /** Gateway of this station's network*/

    struct __attribute__((packed)) FastConnectData {
        uint8_t channel;
        uint8_t bssid[6];
        uint16_t crc16;
    }; /**< Structure used for holding data that need to be saved after deep sleep for Wi-fi.*/

    static FastConnectData rtc_data; /**<Declaration of structure that will be saved in RTC memory.*/

    public:
    WiFiManager(): _sta_netif(nullptr), _instance_any_id(nullptr), _instance_got_ip(nullptr) {};

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
     * //TODO: update docs
     * @brief Initializes and configures Wi-Fi module.
     *
     * Creates and uses handles for network interface, events and event group. Next setups wi-fi module
     * for work in STATION MODE with static IP.
     *
     * @return ESP_OK if initialization succeed.
     * @return Other esp errors if something went wrong.
     */
    esp_err_t init_wifi_station();

    /**
     * @brief Configures static ip for station.
     *
     * Stops _sta_netif DHCP. Next initializes and setup esp_netif_ip_info_t structure and set it up to _sta_netif.
     * This method is created for lowering power usage by the ESP32.
     *
     * @return ESP_OK if initialization succeed.
     * @return Other esp errors if something went wrong.
     */
    esp_err_t set_static_ip();

    /**
     * @brief Deinitialize the Wi-Fi component.
     *
     * Stops and deinitializes Wi-Fi, then destroys the network interface,
     * and finally deletes the default event loop.
     *
     * @return ESP_OK if deinitialization succeed.
     * @return Other ESP errors if deinitialization failed.
     */
    esp_err_t deinit_wifi_station();

private:
    /**
     * @brief Moves channel and bssid information to the structure that is sustained in RTC memory.
     *
     * @return ESP_OK if saving succeed
     * @return Other ESP errors if something gone wrong.
     */
    esp_err_t save_fast_connect_data();

    /**
     * @brief Checks CRC value of the channel and bssid fields inside FastConnectData structure.
     *
     * @return True if FastConnectData structure is not corrupted. Otherwise, it returns False.
     */
    bool is_fast_connect_data_valid();
};


#endif //FIRMWARE_WIFIMANAGER_H