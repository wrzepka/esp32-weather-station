//
// Created by Wiktor on 17.07.2026.
//

#include "../include/WiFiManager.h"

#include <esp_log.h>

#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_rom_crc.h"

namespace {
    /**
     * @brief Auxiliary structure for memory safe EventGroupHandle_t allocation and deallocation.
     */
    struct EventGroup {
        EventGroupHandle_t handle;

        EventGroup() {
            handle = xEventGroupCreate();
        }

        ~EventGroup() {
            if (handle != nullptr) {
                vEventGroupDelete(handle);
            }
        }

        EventGroup(const EventGroup &) = delete;
        EventGroup &operator=(const EventGroup &) = delete;
    };
}


esp_err_t WiFiManager::init_wifi_station() {
    esp_err_t result = ESP_OK;
    if ((result = esp_netif_init()) != ESP_OK) return result;
    if ((result = esp_event_loop_create_default()) != ESP_OK) return result;
    auto network_event_group = EventGroup();

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
        network_event_group.handle,
        &this->_instance_any_id);
    if (result != ESP_OK) return result;

    result = esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &WiFiManager::wifi_event_handler,
        network_event_group.handle,
        &this->_instance_got_ip);
    if (result != ESP_OK) return result;

    wifi_config_t wifi_config = {};
    strlcpy(reinterpret_cast<char *>(wifi_config.sta.ssid), WIFI_SSID, sizeof(wifi_config.sta.ssid));
    strlcpy(reinterpret_cast<char *>(wifi_config.sta.password), WIFI_PASSWORD, sizeof(wifi_config.sta.password));
    wifi_config.sta.scan_method = WIFI_FAST_SCAN;
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    if ((result = esp_wifi_set_mode(WIFI_MODE_STA)) != ESP_OK) return result;
    if ((result = esp_wifi_set_config(WIFI_IF_STA, &wifi_config)) != ESP_OK) return result;
    if ((result = this->set_static_ip()) != ESP_OK) return result;
    if ((result = esp_wifi_start()) != ESP_OK) return result;

    EventBits_t bits = xEventGroupWaitBits(
        network_event_group.handle,
        WIFI_CONNECTED_BIT | WIFI_DISCONNECTED_BIT,
        pdFALSE,
        pdFALSE,
        pdMS_TO_TICKS(MAX_EVENT_GROUP_WAIT_TIME));

    //TODO: change event handlers as local vars?
    if ((result = esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, this->_instance_got_ip)) !=
        ESP_OK) return result;
    if ((result = esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, this->_instance_any_id)) !=
        ESP_OK) return result;

    if (bits & WIFI_CONNECTED_BIT) {
        return ESP_OK;
    }
    return ESP_ERR_TIMEOUT;
}

void WiFiManager::wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    auto s_network_event_group = static_cast<EventGroupHandle_t>(arg);
    static int s_retry_num = 0;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < MAX_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI("WIFI", "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_network_event_group, WIFI_DISCONNECTED_BIT);
        }
        ESP_LOGI("WIFI", "connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        s_retry_num = 0;
        xEventGroupSetBits(s_network_event_group, WIFI_CONNECTED_BIT);
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

esp_err_t WiFiManager::deinit_wifi_station() {
    //TODO: saving to RTC memory (channel, BBSID)
    esp_err_t result = ESP_OK;

    if ((result = esp_wifi_stop()) != ESP_OK) return result;

    if ((result = esp_wifi_deinit()) != ESP_OK) return result;

    esp_netif_destroy_default_wifi(_sta_netif);
    _sta_netif = nullptr;

    if ((result = esp_netif_deinit()) != ESP_OK) return result;

    if ((result = esp_event_loop_delete_default()) != ESP_OK) return result;

    return ESP_OK;
}

RTC_DATA_ATTR WiFiManager::fast_connect_data WiFiManager::rtc_data{};

esp_err_t WiFiManager::save_fast_connect_data() {
    wifi_ap_record_t ap_info = {};
    esp_err_t result = esp_wifi_sta_get_ap_info(&ap_info);
    if (result != ESP_OK) return result;

    memcpy(rtc_data.bssid, ap_info.bssid, sizeof(ap_info.bssid));
    rtc_data.channel = ap_info.primary;
    rtc_data.crc16 = esp_rom_crc16_le(0, reinterpret_cast<uint8_t const *>(&rtc_data), sizeof(rtc_data.bssid) + sizeof(rtc_data.channel));

    return ESP_OK;
}