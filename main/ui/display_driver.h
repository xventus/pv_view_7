// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   display_driver.h
/// @author Petr Vanek
///

#pragma once

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
#include "driver/i2c.h"
#include "esp_lcd_touch_gt911.h"
#include "hardware.h"

/**
 * @class DisplayDriver
 * @brief Driver class for managing the display and input using LVGL and ESP32 peripherals.
 */
class DisplayDriver
{
public:
    /**
     * @brief Constructor for the DisplayDriver class.
     */
    DisplayDriver();

    /**
     * @brief Destructor for the DisplayDriver class. Cleans up resources.
     */
    ~DisplayDriver();

    /**
     * @brief Initializes the display bus and IO configurations.
     * @param needInitI2C - true - initialize I2C
     * @param i2c_port_t  - i2c port
     */
    void initBus(bool needInitI2C, i2c_port_t i2cPort);

    /**
     * @brief Starts the LVGL driver and attaches the display task.
     * 
     * @param usStackDepth Stack size for the display task. Default is 4094 bytes.
     * @param uxPriority Priority of the display task. Default is 2.
     * @param coreId Atach to core Id
     */
    void start(const uint32_t usStackDepth = 4094, UBaseType_t uxPriority = 2, BaseType_t coreId = tskNO_AFFINITY);

    /**
     * @brief Stop detach LVGL driver and stop display task.
     * 
     */
    void stop();


    /**
     * @brief Locks the LVGL UI to prevent simultaneous access.
     * 
     * @param timeout_ms Timeout for the lock in milliseconds. Default is -1 (wait indefinitely).
     * @return true if the lock was acquired successfully, false otherwise.
     */
    bool lock(int timeout_ms = -1);

    /**
     * @brief Unlocks the LVGL UI to allow access by other tasks.
     */
    void unlock();

     /**
     *    @brief Backlight.
     */
    void backLight(bool on);

private:
    /**
     * @brief Event callback for VSYNC.
     * 
     * @param panel Pointer to the LCD panel handle.
     * @param event_data Pointer to the event data structure.
     * @param user_data User-defined data passed to the callback.
     * @return true if the event is handled successfully, false otherwise.
     */
    static bool lvglVsynEvent(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data, void *user_data);

    /**
     * @brief Callback to flush LVGL display buffer to the screen.
     * 
     * @param drv Pointer to the LVGL display driver.
     * @param area Pointer to the area of the display to update.
     * @param color_map Pointer to the color data to render.
     */
    static void lvglFlush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);

    /**
     * @brief Periodic callback to handle LVGL ticks.
     * 
     * @param arg User-defined argument passed to the tick handler.
     */
    static void lvglTick(void *arg);

    /**
     * @brief Callback to read touch input data.
     * 
     * @param drv Pointer to the LVGL input device driver.
     * @param data Pointer to the structure where input data will be stored.
     */
    static void lvglTouch(lv_indev_drv_t *drv, lv_indev_data_t *data);

    /**
     * @brief Task for running LVGL processing in a loop.
     * 
     * @param arg User-defined argument passed to the task.
     */
    static void lvglWorkingTask(void *arg);

    /**
     * @brief Initializes the LCD panel and associated configurations.
     */
    void initLCD();

    /**
     * @brief Initializes the I2C interface for the touch controller.
     * @param needInitI2C - true - initialize I2C
     */
    void initI2C(bool needInitI2C);

    /**
     * @brief Initializes GPIO pins used by the display and touch controller.
     */
    void initGPIO();

    /**
     * @brief Resets the touch controller to ensure proper operation.
     */
    void resetTouch();

    /**
     * @brief Initializes the touch controller and its configuration.
     */
    void initTouch();

private:
    /**
     * @brief Tag used for logging purposes.
     */
    static constexpr const char *TAG = "DD";

    esp_lcd_panel_handle_t _panelHandle{nullptr}; ///< Handle to the LCD panel.
    lv_disp_draw_buf_t _displayBuff{}; ///< LVGL display buffer for rendering.
    lv_disp_drv_t _displayDriver{}; ///< LVGL display driver instance.
    esp_lcd_touch_handle_t _touchHandle{nullptr}; ///< Handle to the touch controller.
    esp_lcd_panel_io_handle_t _touchIOHandle{nullptr}; ///< Handle to the touch I/O interface.
    SemaphoreHandle_t _lvglLock{nullptr}; ///< Semaphore to synchronize access to LVGL.
    lv_indev_drv_t _devDr; ///< LVGL input device driver instance.
    TaskHandle_t _lvglTaskHandle {nullptr}; ///< Handle for the LVGL working task.
    i2c_port_t _i2cPort;
};
