//
// vim: ts=4 et
// Copyright (c) 2023 Petr Vanek, petr@fotoventus.cz
//
/// @file   wifi_scanner.h
/// @author Petr Vanek

#pragma once

#include <string.h>
#include <functional>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
/*

    WiFiScanner wifiScanner;
    wifiScanner.scan();
    wifiScanner.printScanResults();

*/

struct APInfo
{
    char ap_name[33]; ///< Maximu length of AP name
    int8_t rssi;      ///< Rssi
};

using ScanResultCallback = std::function<void(const APInfo &)>;

class WiFiScanner
{
public:
    WiFiScanner()
    {
    }

    /// @brief Initialize Wifi scanner
    /// @param coreInit true if  WiFiClient used as standalone and needs to initialize NVS, Netinf, EventLoop
    /// @return true if success
   void init(bool coreInit = false)
{
    if (coreInit)
    {
        // Initialize NVS
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);

        // Initialize network stack and default event loop
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
    }

    // Check and create Wi-Fi STA interface if not already created
    if (!is_wifi_sta_created())
    {
        _cl = esp_netif_create_default_wifi_sta();
        if (!_cl)
        {
            ESP_LOGE("WiFi", "Failed to create Wi-Fi STA interface");
            return;
        }
    }

    // Initialize Wi-Fi driver
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t err = esp_wifi_init(&cfg);
    if (err == ESP_ERR_WIFI_INIT_STATE)
    {
        ESP_LOGW("WiFi", "Wi-Fi is already initialized");
    }
    else
    {
        ESP_ERROR_CHECK(err);
    }

    // Stop Wi-Fi if running and set mode
    esp_wifi_stop();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI("WiFi", "Wi-Fi initialized successfully in STA mode");
}


    bool is_wifi_sta_created()
    {

        const char *wifi_sta_ifkey = "WIFI_STA_DEF";
        esp_netif_t *netif = esp_netif_get_handle_from_ifkey(wifi_sta_ifkey);
        return (netif != nullptr);
    }

    void down()
    {
        esp_wifi_stop();
        esp_wifi_deinit();
        esp_wifi_set_mode(WIFI_MODE_NULL);
        if (_cl)
        {
            esp_netif_destroy(_cl);
            _cl = nullptr;
        }
    }

    void scan()
    {
        wifi_scan_config_t scan_config = {
            .ssid = nullptr,
            .bssid = nullptr,
            .channel = 0,
            .show_hidden = true,
            .scan_type = WIFI_SCAN_TYPE_ACTIVE,
            .scan_time{{100, 500}, 500}};
        ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true)); // true for block scan
    }

    void debugScanResults()
    {
        uint16_t number = 20;
        wifi_ap_record_t ap_records[number];
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_records));
        for (int i = 0; i < number; i++)
        {
            printf("SSID: %s, RSSI: %d\n", ap_records[i].ssid, ap_records[i].rssi);
        }
    }

    void processResults(ScanResultCallback callback)
    {
        uint16_t number = 20;
        wifi_ap_record_t ap_records[number];
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_records));

        for (int i = 0; i < number; i++)
        {
            APInfo info;
            strncpy(info.ap_name, (const char *)ap_records[i].ssid, sizeof(info.ap_name) - 1);
            info.ap_name[sizeof(info.ap_name) - 1] = '\0';
            info.rssi = ap_records[i].rssi;
            callback(info);
        }
    }

private:
    esp_netif_t *_cl{nullptr};
};
