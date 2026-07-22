//
// Created by Wiktor on 17.07.2026.
//

#include "../include/WiFiManager.h"

#include <esp_log.h>

#include "esp_wifi.h"
#include "nvs_flash.h"


esp_err_t WiFiManager::init_wifi_station() {
    //TODO: FIX MEMORY LEAK (free event group handler)
    esp_err_t result = ESP_OK;
    if ((result = esp_netif_init()) != ESP_OK) return result;
    if ((result = esp_event_loop_create_default()) != ESP_OK) return result;
    this->_s_network_event_group = xEventGroupCreate();

    result = nvs_flash_init();
    if (result == ESP_ERR_NVS_NO_FREE_PAGES || result == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        result = nvs_flash_init();
    }
    if (result != ESP_OK) return result;

    this->_sta_netif = esp_netif_create_default_wifi_sta();

    if (this->_sta_netif == nullptr) {
        return ESP_FAIL;
    }

    wifi_init_config_t init_config = WIFI_INIT_CONFIG_DEFAULT();
    if ((result = esp_wifi_init(&init_config)) != ESP_OK) return result;

    result = esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &WiFiManager::wifi_event_handler,
        this,
        &this->_instance_any_id);
    if (result != ESP_OK) return result;

    result = esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &WiFiManager::wifi_event_handler,
        this,
        &this->_instance_got_ip);
    if (result != ESP_OK) return result;

    wifi_config_t wifi_config = {};
    strlcpy(reinterpret_cast<char *>(wifi_config.sta.ssid), WIFI_SSID, sizeof(wifi_config.sta.ssid));
    strlcpy(reinterpret_cast<char *>(wifi_config.sta.password), WIFI_PASSWORD, sizeof(wifi_config.sta.password));
    wifi_config.sta.scan_method = WIFI_FAST_SCAN;
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    if ((result = esp_wifi_set_mode(WIFI_MODE_STA)) != ESP_OK) return result;
    if ((result = esp_wifi_set_config(WIFI_IF_STA, &wifi_config)) != ESP_OK) return result;
    if ((result = esp_wifi_start()) != ESP_OK) return result;

    EventBits_t bits = xEventGroupWaitBits(
        this->_s_network_event_group,
        WIFI_CONNECTED_BIT | WIFI_DISCONNECTED_BIT,
        pdFALSE,
        pdFALSE,
        pdMS_TO_TICKS(MAX_EVENT_GROUP_WAIT_TIME));

    if ((result = esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, this->_instance_got_ip)) != ESP_OK) return result;
    if ((result = esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, this->_instance_any_id)) != ESP_OK) return result;

    if (bits & WIFI_CONNECTED_BIT) {
        return ESP_OK;
    }
    return ESP_ERR_TIMEOUT;
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

        if (s_retry_num < MAX_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI("WIFI", "retry to connect to the AP");
        } else {
            xEventGroupSetBits(wifi_manager->_s_network_event_group, WIFI_DISCONNECTED_BIT);
        }
        ESP_LOGI("WIFI", "connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        s_retry_num = 0;
        xEventGroupSetBits(wifi_manager->_s_network_event_group, WIFI_CONNECTED_BIT);
    }
}

esp_err_t WiFiManager::set_static_ip() {
    esp_err_t result = esp_netif_dhcpc_stop(this->_sta_netif);
    if (result != ESP_OK) {
        return result;
    }

    esp_netif_ip_info_t ip_info = {};
    esp_err_t address_err = ESP_OK;
    address_err |= esp_netif_str_to_ip4(STATION_IP, &ip_info.ip);
    address_err |= esp_netif_str_to_ip4(STATION_NETMASK, &ip_info.netmask);
    address_err |= esp_netif_str_to_ip4(STATION_GATEWAY, &ip_info.gw);
    if (address_err != ESP_OK) {
        return ESP_ERR_INVALID_ARG;
    }

    result = esp_netif_set_ip_info(this->_sta_netif, &ip_info);
    if (result != ESP_OK) {
        return result;
    }

    return ESP_OK;
}

