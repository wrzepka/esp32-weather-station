//
// Created by Wiktor on 17.07.2026.
//

#include "../include/WiFiManager.h"

#include <esp_log.h>

#include "esp_wifi.h"
#include "nvs_flash.h"


void WiFiManager::init_wifi_station() {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    this->_s_network_event_group = xEventGroupCreate();

    esp_err_t result = nvs_flash_init();
    if (result == ESP_ERR_NVS_NO_FREE_PAGES || result == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        result = nvs_flash_init();
    }
    ESP_ERROR_CHECK(result);

    this->_sta_netif = esp_netif_create_default_wifi_sta();

    if (this->_sta_netif == nullptr) {
        ESP_LOGI("WIFI", "Failed to create a WiFi network interface");
    }

    wifi_init_config_t init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init_config));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &WiFiManager::wifi_event_handler,
        this,
        &this->_instance_any_id));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &WiFiManager::wifi_event_handler,
        this,
        &this->_instance_got_ip));

    wifi_config_t wifi_config = {};
    strlcpy(reinterpret_cast<char *>(wifi_config.sta.ssid), "PocoF5", sizeof(wifi_config.sta.ssid));
    strlcpy(reinterpret_cast<char *>(wifi_config.sta.password), "12345678", sizeof(wifi_config.sta.password));
    wifi_config.sta.scan_method = WIFI_FAST_SCAN;
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    EventBits_t bits = xEventGroupWaitBits(
        this->_s_network_event_group,
        BIT0 | BIT1,
        pdFALSE,
        pdFALSE,
        pdMS_TO_TICKS(5000));

    static const char *TAG = "WIFI";

    if (bits & BIT0) {
        ESP_LOGI(TAG, "connected to AP");
    } else if (bits & BIT1) {
        ESP_LOGI(TAG, "Failed to connect!");
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, this->_instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, this->_instance_any_id));
}

void WiFiManager::wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    WiFiManager *wifi_manager = static_cast<WiFiManager *>(arg);
    static int s_retry_num = 0;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        wifi_manager->set_static_ip();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // auto* disconnected_data = static_cast<wifi_event_sta_disconnected_t*>(event_data);
        // while (true) {
        //     ESP_LOGE("WIFI_ERROR", "%d", disconnected_data->reason);
        // }

        if (s_retry_num < 5) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI("WIFI", "retry to connect to the AP");
        } else {
            xEventGroupSetBits(wifi_manager->_s_network_event_group, BIT1);
        }
        ESP_LOGI("WIFI", "connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        s_retry_num = 0;
        xEventGroupSetBits(wifi_manager->_s_network_event_group, BIT0);
    }
}

void WiFiManager::set_static_ip() {
    if (esp_netif_dhcpc_stop(this->_sta_netif) != ESP_OK) {
        ESP_LOGE("WIFI", "Failed to stop DHCP server");
        return;
    }

    esp_netif_ip_info_t ip_info = {};
    esp_netif_str_to_ip4("10.246.161.67", &ip_info.ip);
    esp_netif_str_to_ip4("255.255.255.0", &ip_info.netmask);
    esp_netif_str_to_ip4("10.246.161.1", &ip_info.gw);

    if (esp_netif_set_ip_info(this->_sta_netif, &ip_info) != ESP_OK) {
        ESP_LOGE("WIFI", "Failed to set ip info");
        return;
    }

    ESP_LOGI("WIFI", "Successfully set up static ip.");
}

