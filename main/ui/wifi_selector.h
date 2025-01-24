// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   wifi_selector.h
/// @author Petr Vanek
///

#pragma once

#include "lvgl.h"
#include "wifi_scanner.h"
#include "ui_style.h"
#include <vector>
#include <string>

/**
 * @class WifiSsidSelector
 * @brief Class for managing a WiFi SSID selection panel.
 */
class WifiSsidSelector
{
private:
    static constexpr const char *TAG = "WFSEL";
    lv_obj_t *_container;                                     ///< Container for the WiFi selector UI.
    lv_obj_t *_list;                                          ///< List object for displaying available WiFi SSIDs.
    lv_obj_t *_scan_btn;                                      ///< Button for scanning WiFi networks (not currently used).
    static constexpr lv_coord_t _margin = 10;                 ///< Margin for spacing within the UI.
    TaskHandle_t _scanTaskHandle = nullptr;                   ///< Task handle for periodic WiFi scanning.
    WiFiScanner _wifiScanner;                                 ///< WiFi scanner instance for scanning networks.
    std::function<void(const std::string &)> _onWifiSelected; ///< Callback for when a WiFi SSID is selected.

public:
    /**
     * @brief Constructor for WifiSsidSelector.
     * @param parent The parent LVGL object to attach this selector to.
     * @param width Width of the selector container.
     * @param height Height of the selector container.
     * @param x X-coordinate of the container position.
     * @param y Y-coordinate of the container position.
     */
    WifiSsidSelector(lv_obj_t *parent, lv_coord_t width = 400, lv_coord_t height = 400, lv_coord_t x = 10, lv_coord_t y = 10)
    {
        init(parent, width, height, x, y);
    }

    /**
     * @brief Destructor for WifiSsidSelector. Cleans up allocated resources.
     */
    ~WifiSsidSelector()
    {
        stopPeriodicScan();
        if (_container)
            lv_obj_del(_container);
    }

    /**
     * @brief Sets the background color of the container and list.
     * @param color The color to set.
     * @param opa Opacity of the background color. Default is fully opaque.
     */
    void setBackgroundColor(lv_color_t color, lv_opa_t opa = LV_OPA_COVER)
    {
        if (_container)
        {
            lv_obj_set_style_bg_color(_container, color, 0);
            lv_obj_set_style_bg_opa(_container, opa, 0);
        }

        if (_list)
        {
            lv_obj_set_style_bg_color(_list, color, 0);
        }
    }

    /**
     * @brief Sets the callback for when a WiFi SSID is selected.
     * @param callback The callback function to invoke.
     */
    void withOnWifiSelectedCallback(std::function<void(const std::string &)> callback)
    {
        _onWifiSelected = callback;
    }

    /**
     * @brief Starts periodic scanning for WiFi networks.
     *        Scanning occurs every 10 seconds.
     */
    void startPeriodicScan()
    {
        if (_scanTaskHandle == nullptr)
        {
            xTaskCreate(
                [](void *param)
                {
                    WifiSsidSelector *selector = static_cast<WifiSsidSelector *>(param);
                    while (true)
                    {
                        selector->scanAndDisplaySsids();
                        vTaskDelay(pdMS_TO_TICKS(10000));
                    }
                },
                "WiFiScanTask",
                4096,
                this,
                1,
                &_scanTaskHandle);
        }
    }

    /**
     * @brief Stops the periodic WiFi scanning task.
     */
    void stopPeriodicScan()
    {
        if (_scanTaskHandle != nullptr)
        {
            vTaskDelete(_scanTaskHandle);
            _scanTaskHandle = nullptr;
        }
    }

private:
    /**
     * @brief Initializes the selector UI components.
     * @param parent The parent LVGL object to attach this selector to.
     * @param width Width of the selector container.
     * @param height Height of the selector container.
     * @param x X-coordinate of the container position.
     * @param y Y-coordinate of the container position.
     */
    void init(lv_obj_t *parent, lv_coord_t width, lv_coord_t height, lv_coord_t x, lv_coord_t y)
    {
        // Create container
        _container = lv_obj_create(parent);
        lv_obj_set_size(_container, width, height);
        lv_obj_align(_container, LV_ALIGN_DEFAULT, x, y);

        // Set container border style
        lv_obj_set_style_border_width(_container, UIStyle::BorderPanelW, 0);
        lv_obj_set_style_border_color(_container, lv_color_hex(UIStyle::BorderPanel), 0);
        lv_obj_set_style_border_opa(_container, LV_OPA_COVER, 0);

        // Set container background style
        lv_obj_set_style_bg_color(_container, lv_color_hex(UIStyle::BackgroundContent), 0);
        lv_obj_set_style_bg_opa(_container, LV_OPA_COVER, 0);

        // Set padding for the container
        lv_obj_set_style_pad_all(_container, _margin, 0);

        // Create list inside container
        _list = lv_list_create(_container);
        lv_obj_set_size(_list, width - (3 * _margin), height - (3 * _margin));

        // Set list background style
        lv_obj_set_style_bg_color(_list, lv_color_hex(UIStyle::BackgroundContent), 0);
        lv_obj_set_style_bg_opa(_list, LV_OPA_COVER, 0);

        lv_obj_set_style_border_width(_list, 0, 0); // No border for the list

        // Set scroll direction and alignment for the list
        lv_obj_set_scroll_dir(_list, LV_DIR_VER);
        lv_obj_align(_list, LV_ALIGN_TOP_LEFT, 0, 0);

        lv_obj_t *noWiFiLabel = lv_label_create(_list);
        lv_label_set_text(noWiFiLabel, "Looking for available WiFi ...");
        lv_obj_set_style_text_align(noWiFiLabel, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(noWiFiLabel, LV_ALIGN_CENTER, 0, 0); // Center the label
    }

    /**
     * @brief Scans for available WiFi networks and displays them in the list.
     */
    void scanAndDisplaySsids()
    {
        _wifiScanner.init();
        _wifiScanner.scan();
        ESP_LOGI(TAG, "****>");

        {
            lv_obj_t *child;
            while ((child = lv_obj_get_child(_list, -1)) != nullptr)
            {
                lv_obj_del(child);
            }
        }

        _wifiScanner.processResults([this](const APInfo &info)
                                    {
    lv_obj_t *btn = lv_list_add_btn(_list, LV_SYMBOL_WIFI, info.ap_name);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(btn, 0, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(UIStyle::BackgroundContent), LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, LV_STATE_PRESSED);

    // callback
    lv_obj_add_event_cb(btn, [](lv_event_t *e) {
        lv_obj_t *btn = lv_event_get_target(e);
        WifiSsidSelector *selector = static_cast<WifiSsidSelector *>(lv_event_get_user_data(e));
        if (!selector) {
            ESP_LOGE(TAG, "User data is null");
            return;
        }
        const char *selected_ssid = lv_list_get_btn_text(lv_obj_get_parent(btn), btn);
        if (!selected_ssid) {
            ESP_LOGE(TAG, "Failed to get SSID text");
            return;
        }
        ESP_LOGI(TAG, "Vybrané SSID: %s", selected_ssid);

        if (selector->_onWifiSelected) {
            selector->_onWifiSelected(selected_ssid);
        } else {
            ESP_LOGW(TAG, "Callback _onWifiSelected is not set");
        }
    }, LV_EVENT_CLICKED, this); });

        _wifiScanner.down();
    }
};
