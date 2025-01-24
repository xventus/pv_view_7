// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   main_screen.cpp
/// @author Petr Vanek
///

#include "main_screen.h"
#include "esp_log.h"
#include "ui/screen_manager.h"
#include "setting_screen.h"
#include "ui/ui.h"
#include "utils.h"
#include "application.h"

#include "ui/icons8_solar_panel_48.h"
#include "ui/icons8_car_battery_48.h"
#include "ui/icons8_telephone_pole_48.h"
#include "ui/icons8_home_48.h"
#include "ui/icons8_generator_32.h"
#include "ui/icons8_error_30.h"
#include "ui/icons8_wifi_24.h"

MainScreen::MainScreen() {}

ScreenType MainScreen::getType() const
{
    return ScreenType::Main;
}

bool MainScreen::init()
{
    bool rc = false;

    do
    {
        if (_screen && lv_obj_is_valid(_screen))
        {
            ESP_LOGE(TAG, "Screen already initialized");
            break;
        }

        _screen = lv_obj_create(nullptr);
        if (!_screen)
            break;

        lv_obj_set_style_bg_color(_screen, lv_color_hex(UIStyle::BackgroundScreen), 0);
        lv_obj_set_style_bg_opa(_screen, LV_OPA_COVER, 0);
        createFrames();
        rc = true;
    } while (false);
    return rc;
}

void MainScreen::createFrames()
{
    const int frameSolWidth = 250;   // solar, batt
    const int frameSolHeight = 160;  // solar, batt
    const int frameGridWidth = 250;  // grid, home
    const int frameGridHeight = 160; // grid, home
    const int distVert = 4;
    const int dist = 2;
    const int distOver = 50;
    const int energyProgressWidth = 200;
    const int diffIcon = -15;
    const int diffIcon2 = 5;
    const int ledSize = 10;
    const int frameOverviewWidth = 292;
    const int frameOverviewHeight = 260;
    const int overviewLabell1l3 = -100;

    // Solar Panels
    _solarPanelFrame = UI::createFrame(_screen, frameSolWidth, frameSolHeight, LV_ALIGN_TOP_LEFT, dist, dist, lv_color_hex(UIStyle::Black), lv_color_hex(UIStyle::White));
    _solarPanelTotalPowerLabel = UI::addLabel(_solarPanelFrame, "----", LV_ALIGN_CENTER, 0, -dist, &lv_font_montserrat_40);
    UI::addIcon(_solarPanelFrame, &icons8_solar_panel_48, LV_ALIGN_TOP_LEFT, diffIcon, diffIcon);
    _solarPanelString1Label = UI::addLabel(_solarPanelFrame, "----", LV_ALIGN_BOTTOM_LEFT, 0, 0, &lv_font_montserrat_20);
    _solarPanelString2Label = UI::addLabel(_solarPanelFrame, "----", LV_ALIGN_BOTTOM_RIGHT, 0, 0, &lv_font_montserrat_20);
    lv_obj_set_scrollbar_mode(_solarPanelFrame, LV_SCROLLBAR_MODE_OFF);

    // Battery
    _batteryFrame = UI::createFrame(_screen, frameSolWidth, frameSolHeight, LV_ALIGN_TOP_RIGHT, -dist, dist, lv_color_hex(UIStyle::Black), lv_color_hex(UIStyle::White));
    _batteryPercentageLabel = UI::addLabel(_batteryFrame, "---", LV_ALIGN_CENTER, 0, 0, &lv_font_montserrat_40);
    UI::addIcon(_batteryFrame, &icons8_car_battery_48, LV_ALIGN_TOP_LEFT, diffIcon, diffIcon);
    _batteryPowerLabel = UI::addLabel(_batteryFrame, "----", LV_ALIGN_BOTTOM_LEFT, 0, 0, &lv_font_montserrat_20);
    _batteryTempLabel = UI::addTemperatureLabel(_batteryFrame, "----", LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    lv_obj_set_scrollbar_mode(_batteryFrame, LV_SCROLLBAR_MODE_OFF);

    // grid
    _gridFrame = UI::createFrame(_screen, frameGridWidth, frameGridHeight, LV_ALIGN_TOP_LEFT, dist, frameSolHeight + distVert, lv_color_hex(UIStyle::Black), lv_color_hex(UIStyle::White));
    _gridLabel = UI::addLabel(_gridFrame, "----", LV_ALIGN_CENTER, 0, -dist, &lv_font_montserrat_40);
    UI::addIcon(_gridFrame, &icons8_telephone_pole_48, LV_ALIGN_TOP_LEFT, diffIcon, diffIcon);
    _gridLed = lv_led_create(_gridFrame);
    lv_obj_set_size(_gridLed, ledSize, ledSize); // Set LED size
    lv_obj_align(_gridLed, LV_ALIGN_BOTTOM_LEFT, 5, -5);
    lv_led_set_color(_gridLed, lv_color_hex(0x00FF00));
    lv_obj_set_scrollbar_mode(_gridFrame, LV_SCROLLBAR_MODE_OFF);

    // Consumption - home
    _consumptionFrame = UI::createFrame(_screen, frameGridWidth, frameGridHeight, LV_ALIGN_TOP_RIGHT, -dist, frameSolHeight + distVert, lv_color_hex(UIStyle::Black), lv_color_hex(UIStyle::White));
    _consumptionLabel = UI::addLabel(_consumptionFrame, "----", LV_ALIGN_CENTER, 0, -dist, &lv_font_montserrat_40);
    UI::addIcon(_consumptionFrame, &icons8_home_48, LV_ALIGN_TOP_LEFT, diffIcon, diffIcon);
    lv_obj_set_scrollbar_mode(_consumptionFrame, LV_SCROLLBAR_MODE_OFF);

    // Overview
    _overviewFrame = UI::createFrame(_screen, frameOverviewWidth, frameOverviewHeight, LV_ALIGN_TOP_MID, 0, dist, lv_color_hex(UIStyle::Black), lv_color_hex(UIStyle::White));
    _overviewPowerLabel = UI::addLabel(_overviewFrame, "----", LV_ALIGN_CENTER, 0, -distOver, &lv_font_montserrat_40);
    _overviewTempLabel = UI::addTemperatureLabel(_overviewFrame, "----", LV_ALIGN_BOTTOM_MID, 0, 5);
    UI::addIcon(_overviewFrame, &icons8_generator_32, LV_ALIGN_TOP_LEFT, diffIcon2, 0);
    _l1 = UI::addLabel(_overviewFrame, "----", LV_ALIGN_BOTTOM_LEFT, 0, overviewLabell1l3, &lv_font_montserrat_20);
    _l2 = UI::addLabel(_overviewFrame, "----", LV_ALIGN_BOTTOM_MID, 0, overviewLabell1l3, &lv_font_montserrat_20);
    _l3 = UI::addLabel(_overviewFrame, "----", LV_ALIGN_BOTTOM_RIGHT, 0, overviewLabell1l3, &lv_font_montserrat_20);
    _mode = UI::addLabel(_overviewFrame, "----", LV_ALIGN_CENTER, 0, 50, &lv_font_montserrat_20);
    lv_obj_set_scrollbar_mode(_overviewFrame, LV_SCROLLBAR_MODE_OFF);

    // Energy Bar
    _energyBarFrame = UI::createFrame(_screen, frameGridWidth, 150, LV_ALIGN_BOTTOM_LEFT, dist, -4, lv_color_hex(UIStyle::Black), lv_color_hex(UIStyle::White));
    _energyBarLabel = UI::addLabel(_energyBarFrame, "----", LV_ALIGN_TOP_MID, 0, -dist, &lv_font_montserrat_40);
    _energyBar = UI::addBar(_energyBarFrame, energyProgressWidth, 20, LV_ALIGN_CENTER, 0, 10);
    lv_obj_set_scrollbar_mode(_energyBarFrame, LV_SCROLLBAR_MODE_OFF);
    _hdoLed = lv_led_create(_energyBarFrame);
    lv_obj_set_size(_hdoLed, ledSize, ledSize); // Set LED size
    lv_obj_align(_hdoLed, LV_ALIGN_BOTTOM_LEFT, 5, -5);
    lv_led_set_color(_hdoLed, lv_color_hex(0x00FF00));

    _timePanelFrame = UI::createFrame(_screen, frameGridWidth, 150, LV_ALIGN_BOTTOM_RIGHT, -dist, -4, lv_color_hex(UIStyle::Black), lv_color_hex(UIStyle::White));
    _timeLabel = UI::addLabel(_timePanelFrame, "--:--", LV_ALIGN_CENTER, 0, 0, &lv_font_montserrat_40);
    lv_obj_align(_timeLabel, LV_ALIGN_CENTER, 0, -30);
    _dateLabel = UI::addLabel(_timePanelFrame, "--.--.----", LV_ALIGN_CENTER, 0, 20, &lv_font_montserrat_40);
    lv_obj_align(_dateLabel, LV_ALIGN_CENTER, 0, 30);
    lv_obj_set_scrollbar_mode(_timePanelFrame, LV_SCROLLBAR_MODE_OFF);

    createBarGraph(frameOverviewWidth, 212, -4, "", lv_color_hex(0x007BFF));

    enableOverviewClick();
}

void MainScreen::enableOverviewClick()
{
    if (_overviewFrame)
    {
        lv_obj_add_event_cb(_overviewFrame, [](lv_event_t *e)
                            {
                MainScreen *dashboard = static_cast<MainScreen *>(lv_event_get_user_data(e));
                if (!ScreenManager::getInstance()->showScreenByType(ScreenType::Setting))
                {
                    Application::getInstance()->getWifiTask()->switchMode(WifiTask::Mode::Stop);

                    ScreenManager::getInstance()->addScreen(std::make_unique<SettingScreen>());
                    ScreenManager::getInstance()->showScreenByType(ScreenType::Setting);
                } }, LV_EVENT_CLICKED, this);
        ESP_LOGI(TAG, "Overview click event enabled.");
    }
    else
    {
        ESP_LOGI(TAG, "_overviewFrame is null.");
    }
}

void MainScreen::createBarGraph(int frameOverviewWidth, int frameOverviewHeight, int top, const char *title, lv_color_t barColor)
{
    // Create a frame for the bar graph
    _barGraphFrame = UI::createFrame(_screen, frameOverviewWidth, frameOverviewHeight, LV_ALIGN_BOTTOM_MID, 0, top, lv_color_hex(UIStyle::Black), lv_color_hex(UIStyle::White));
    lv_obj_set_scrollbar_mode(_barGraphFrame, LV_SCROLLBAR_MODE_OFF);

    // Add a title label to the frame
    _chartTitleLabel = UI::addLabel(_barGraphFrame, title, LV_ALIGN_TOP_MID, 0, -10, &lv_font_montserrat_14);

    // Create the bar graph
    _chart = lv_chart_create(_barGraphFrame);
    lv_obj_set_size(_chart, 260, 120); // Set the size of the bar graph
    lv_obj_align(_chart, LV_ALIGN_CENTER, 0, 20);

    // Set the Y-axis range
    lv_chart_set_range(_chart, LV_CHART_AXIS_PRIMARY_Y, 10, 10000); // Assuming max value is 12000W

    // Configure the chart as a bar graph
    lv_chart_set_type(_chart, LV_CHART_TYPE_BAR);
    lv_obj_set_style_line_opa(_chart, LV_OPA_TRANSP, LV_PART_ITEMS);
    lv_obj_set_style_line_width(_chart, 0, LV_PART_ITEMS);
    lv_obj_set_style_pad_column(_chart, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_gap(_chart, 4, LV_PART_MAIN);
    lv_chart_set_point_count(_chart, 24); // 24 hours of data

    lv_chart_set_div_line_count(_chart, 10, 0);

    // Remove vertical grid lines
    lv_obj_set_style_pad_column(_chart, 0, LV_PART_MAIN);

    // Create the series and assign it to _chartSeries
    _chartSeries = lv_chart_add_series(_chart, barColor, LV_CHART_AXIS_PRIMARY_Y);

    // Populate the series with initial data
    for (int i = 0; i < 24; i++)
    {
        lv_chart_set_value_by_id(_chart, _chartSeries, i, _dataSets[0].data[i]);
    }

    // Maximum info
    _maxLabel = lv_label_create(lv_obj_get_parent(_chart));
    lv_label_set_text(_maxLabel, "--");
    lv_obj_set_style_text_font(_maxLabel, &lv_font_montserrat_12, 0);

    lv_obj_set_pos(_maxLabel, lv_obj_get_x(_chart), lv_obj_get_y(_chart) + 20);

    // Make the chart clickable
    lv_obj_add_flag(_chart, LV_OBJ_FLAG_CLICKABLE);

    // Add an event listener for the chart
    lv_obj_add_event_cb(_barGraphFrame, [](lv_event_t *e)
                        {
                MainScreen *dashboard = static_cast<MainScreen *>(lv_event_get_user_data(e));
                ESP_LOGI(TAG,"Chart clicked");
        dashboard->onChartClick(); }, LV_EVENT_CLICKED, this);

    lv_obj_add_event_cb(_chart, [](lv_event_t *e)
                        {
                MainScreen *dashboard = static_cast<MainScreen *>(lv_event_get_user_data(e));
                ESP_LOGI(TAG,"Chart clicked");
        dashboard->onChartClick(); }, LV_EVENT_CLICKED, this);

    // --------- Create overview

    _totalSolLabel = UI::addLabel(_barGraphFrame, "----", LV_ALIGN_TOP_LEFT, 0, 20, &lv_font_montserrat_20);
    _totalSol = UI::addIcon(_barGraphFrame, &icons8_solar_panel_48, LV_ALIGN_BOTTOM_LEFT, 0, -15);

    _dayConsumpLabel = UI::addLabel(_barGraphFrame, "----", LV_ALIGN_TOP_RIGHT, 0, 20, &lv_font_montserrat_20);
    _totalCons = UI::addIcon(_barGraphFrame, &icons8_home_48, LV_ALIGN_BOTTOM_RIGHT, 0, -15);

    _temperatureOut = UI::addLabel(_barGraphFrame, "--", LV_ALIGN_TOP_MID, 0, 60, &lv_font_montserrat_40);

    graphDisplay(false);
}

void MainScreen::graphDisplay(bool hideGraph)
{
    if (hideGraph)
    {
        lv_obj_add_flag(_chartTitleLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(_chart, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(_maxLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(_totalSolLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(_dayConsumpLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(_totalSol, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(_totalCons, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(_temperatureOut, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_clear_flag(_chartTitleLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(_chart, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(_maxLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(_totalSolLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(_dayConsumpLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(_totalSol, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(_totalCons, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(_temperatureOut, LV_OBJ_FLAG_HIDDEN);
    }
}

void MainScreen::onChartClick()
{
    ESP_LOGI(TAG, "onChartClick called");

    if (!_chart)
    {
        ESP_LOGE(TAG, "Error: _chart is null");
        return;
    }

    if (!_chartSeries)
    {
        ESP_LOGE(TAG, "Error: _chartSeries is null");
        return;
    }

    _currentDataSetIndex = (_currentDataSetIndex + 1) % 3;
    updateChart();

    ESP_LOGI(TAG, "onChartClick: %d\n", _currentDataSetIndex);
}

void MainScreen::updateChartRange()
{
    if (!_chart || !_chartSeries)
    {
        ESP_LOGE(TAG, "Error: _chart or _chartSeries is null");
        return;
    }

    int minValue = 0;
    int maxValue = 0;

    for (int i = 0; i < 24; i++)
    {
        int value = _dataSets[_currentDataSetIndex].data[i];
        if (value == LV_CHART_POINT_NONE)
            continue;
        if (value < minValue)
            minValue = value;
        if (value > maxValue)
            maxValue = value;
    }

    lv_chart_set_range(_chart, LV_CHART_AXIS_PRIMARY_Y, minValue, maxValue);
    lv_chart_refresh(_chart);

    if (_maxLabel)
    {
        std::string mx = "Max: ";
        if (maxValue == INT16_MIN || minValue == INT16_MAX)
            _maxLabel = 0;
        mx += Utils::formatPower(maxValue, "W", "h");
        lv_label_set_text(_maxLabel, mx.c_str());
    }

    // ESP_LOGI(TAG, "Chart range updated: Min=%d, Max=%d", minValue, maxValue);
    // ESP_LOGI(TAG, "Chart range index: %d\n", _currentDataSetIndex);
}

void MainScreen::updateChart()
{
    if (_currentDataSetIndex == 2)
    {
        graphDisplay(true);
    }
    else
    {
        graphDisplay(false);
        lv_label_set_text(_chartTitleLabel, _dataSets[_currentDataSetIndex].description);
        lv_chart_set_series_color(_chart, _chartSeries, _dataSets[_currentDataSetIndex].color);

        for (int i = 0; i < 24; i++)
        {
            lv_chart_set_value_by_id(_chart, _chartSeries, i, _dataSets[_currentDataSetIndex].data[i]);
        }

        updateChartRange();
        // ESP_LOGI(TAG, "Dataset switched to: %s\n", _dataSets[_currentDataSetIndex].description);
    }
}

void MainScreen::down()
{
    if (_screen && lv_obj_is_valid(_screen))
    {
        lv_obj_del(_screen);
        _screen = nullptr;
    }

    ESP_LOGI(TAG, "MainScreen resources released.");
}

void MainScreen::show()
{
    if (_screen)
    {
        DDLockGuard lock;
        lv_scr_load(_screen);
        clearAllDataSets();
    }
}

void MainScreen::message(std::string_view msg)
{
    if (!msg.empty())
    {
        DDLockGuard lock;
        lv_obj_t *mbox1 = lv_msgbox_create(NULL, "SD content", msg.data(), nullptr, true);
        lv_obj_center(mbox1);
    }
}

void MainScreen::setNowWifi()
{
    const Message msg = {
        &icons8_wifi_24,
        "No connection"};
    updateEnergyMessage(msg);
    showEnergyMessage();
}

void MainScreen::setNoMqtt()
{
    const Message msg = {
        &icons8_error_30,
        "No MQTT"};
    updateEnergyMessage(msg);
    showEnergyMessage();
}

void MainScreen::updateEnergyMessage(const Message &message)
{
    if (!_messageIcon)
    {
        _messageIcon = lv_img_create(_energyBarFrame);
        lv_obj_align(_messageIcon, LV_ALIGN_TOP_MID, 0, 0);
    }
    if (!_messageLabel)
    {
        _messageLabel = lv_label_create(_energyBarFrame);
        lv_obj_set_style_text_font(_messageLabel, &lv_font_montserrat_14, 0);
        lv_obj_align(_messageLabel, LV_ALIGN_BOTTOM_MID, 0, 0);
    }

    lv_img_set_src(_messageIcon, message.icon);
    lv_label_set_text(_messageLabel, message.text);
}

void MainScreen::showEnergyBar()
{
    if (_energyBarFrame)
    {
        lv_obj_set_style_bg_color(_energyBarFrame, lv_color_hex(UIStyle::White), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(_energyBarFrame, LV_OPA_COVER, LV_PART_MAIN);
    }

    if (_energyBar)
    {
        lv_obj_clear_flag(_energyBar, LV_OBJ_FLAG_HIDDEN);
    }

    if (_energyBarLabel)
    {
        lv_obj_clear_flag(_energyBarLabel, LV_OBJ_FLAG_HIDDEN);
    }

    if (_messageIcon)
    {
        lv_obj_add_flag(_messageIcon, LV_OBJ_FLAG_HIDDEN);
    }
    if (_messageLabel)
    {
        lv_obj_add_flag(_messageLabel, LV_OBJ_FLAG_HIDDEN);
    }
}

void MainScreen::showEnergyMessage()
{

    if (_energyBarFrame)
    {
        lv_obj_set_style_bg_color(_energyBarFrame, lv_color_hex(0xFFCCCC), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(_energyBarFrame, LV_OPA_COVER, LV_PART_MAIN);
    }

    if (_energyBar)
    {
        lv_obj_add_flag(_energyBar, LV_OBJ_FLAG_HIDDEN);
    }

    if (_energyBarLabel)
    {
        lv_obj_add_flag(_energyBarLabel, LV_OBJ_FLAG_HIDDEN);
    }

    if (_messageIcon)
    {
        lv_obj_clear_flag(_messageIcon, LV_OBJ_FLAG_HIDDEN);
    }
    if (_messageLabel)
    {
        lv_obj_clear_flag(_messageLabel, LV_OBJ_FLAG_HIDDEN);
    }
}

void MainScreen::clearAllDataSets()
{
    DDLockGuard lock;
   
    for (int datasetIndex = 0; datasetIndex < 2; datasetIndex++)
    {
        for (int hour = 0; hour < 24; hour++)
        {
            _dataSets[datasetIndex].data[hour] = LV_CHART_POINT_NONE;
        }
    }

    ESP_LOGI(TAG, "All data sets cleared\n");

    // Optionally update the chart if it's active
    if (_chartSeries && _chart)
    {
        for (int i = 0; i < 24; i++)
        {
            lv_chart_set_value_by_id(_chart, _chartSeries, i, LV_CHART_POINT_NONE);
        }

        updateChart();
        updateChartRange();
    }
}

void MainScreen::solaxUpdate(const SolarData &sol)
{

     DDLockGuard lock;

    if (_totalSolLabel)
        lv_label_set_text(_totalSolLabel, Utils::formatPower(sol.sol, "W", "h").c_str());
    
    if (_dayConsumpLabel)
        lv_label_set_text(_dayConsumpLabel, Utils::formatPower(sol.cons, "W", "h").c_str());

    // inverter
    if (_mode)
    {
        switch (sol.mode)
        {
        case 1:
            lv_label_set_text(_mode, "Checking");
            break;
        case 2:
            lv_label_set_text(_mode, "Normal");
            break;
        case 3:
            lv_label_set_text(_mode, "Fault");
            break;
        case 4:
            lv_label_set_text(_mode, "Permanent Fault");
            break;
        case 5:
            lv_label_set_text(_mode, "Update");
            break;
        case 6:
            lv_label_set_text(_mode, "Off-grid waiting");
            break;
        case 7:
            lv_label_set_text(_mode, "Off-grid");
            break;
        case 8:
            lv_label_set_text(_mode, "Self Testing");
            break;
        case 9:
            lv_label_set_text(_mode, "Idle");
            break;
        case 10:
            lv_label_set_text(_mode, "Standby");
            break;
        default:
            lv_label_set_text(_mode, "Unknown");
            break;
        }
    }
    
    if (_overviewPowerLabel)  lv_label_set_text(_overviewPowerLabel, Utils::formatPower(sol.inverterTotal).c_str());
    if (_l1)  lv_label_set_text(_l1, Utils::formatPower(sol.gridPowerR).c_str());
    if (_l2)  lv_label_set_text(_l2, Utils::formatPower(sol.gridPowerS).c_str());
    if (_l3)  lv_label_set_text(_l3, Utils::formatPower(sol.gridPowerT).c_str());
    
    if (_overviewTempLabel) 
    { 
        lv_label_set_text(_overviewTempLabel, Utils::formatTemperature(sol.invTemp).c_str());
        updateTemperatureTextColor(_overviewTempLabel, sol.invTemp);
    }

    // date & time
    if (_timeLabel && _dateLabel && lv_obj_is_valid(_timeLabel) && lv_obj_is_valid(_dateLabel))
    {
        auto [dt, tm] = Utils::getDateTime();
        lv_label_set_text(_timeLabel, tm.c_str());
        lv_label_set_text(_dateLabel, dt.c_str());
    }

    // Strings - PV
    int totalPower = sol.powerDC1 + sol.powerDC2;

    if (_solarPanelTotalPowerLabel) lv_label_set_text(_solarPanelTotalPowerLabel, Utils::formatPower(totalPower).c_str());
    if (_solarPanelString1Label) lv_label_set_text(_solarPanelString1Label, Utils::formatPower(sol.powerDC1).c_str());
    if (_solarPanelString2Label) lv_label_set_text(_solarPanelString2Label, Utils::formatPower(sol.powerDC2).c_str());

    if (totalPower > 100)
    {
        if (_solarPanelFrame) {
            lv_obj_set_style_border_color(_solarPanelFrame, lv_color_hex(UIStyle::Black), 0);
            lv_obj_set_style_bg_color(_solarPanelFrame, lv_color_hex(UIStyle::LtGreen), 0);
        }
    }
    else
    {
        if (_solarPanelFrame) {
            lv_obj_set_style_border_color(_solarPanelFrame, lv_color_hex(UIStyle::Black), 0);
            lv_obj_set_style_bg_color(_solarPanelFrame, lv_color_hex(UIStyle::White), 0);
        }
    }

    // Battery 
    if (_batteryPercentageLabel) lv_label_set_text(_batteryPercentageLabel, Utils::formatPower(sol.batteryCapacity,"","%").c_str());
    if (_batteryPowerLabel) lv_label_set_text(_batteryPowerLabel, Utils::formatPower(sol.batteryChargePower).c_str());
    if (_batteryTempLabel) lv_label_set_text(_batteryTempLabel, Utils::formatPower(sol.batteryTemperature,"","°C").c_str());

    if (_batteryFrame)
    {
        // Change the frame color based on power
        if (sol.batteryChargePower == 0)
        {
            // Black for zero power
            lv_obj_set_style_border_color(_batteryFrame, lv_color_hex(UIStyle::Black), 0);
            lv_obj_set_style_bg_color(_batteryFrame, lv_color_hex(UIStyle::White), 0);
        }
        else if (sol.batteryChargePower > 100)
        {
            // Dark green for positive power
            lv_obj_set_style_border_color(_batteryFrame, lv_color_hex(UIStyle::Black), 0);
            lv_obj_set_style_bg_color(_batteryFrame, lv_color_hex(UIStyle::LtGreen), 0);
        }
        else if (sol.batteryChargePower < 100)
        {
            // Dark red for negative power
            lv_obj_set_style_border_color(_batteryFrame, lv_color_hex(UIStyle::Black), 0);
            lv_obj_set_style_bg_color(_batteryFrame, lv_color_hex(UIStyle::LtRed), 0);
        }
    }

    // Consumption
    if (_consumptionLabel) lv_label_set_text(_consumptionLabel, Utils::formatPower(sol.consumption).c_str());
    

    // Grid
     if (_gridLabel) lv_label_set_text(_gridLabel, Utils::formatPower(sol.feedinPower).c_str());  

    if (_gridLed)
    {
        if (sol.onGrid)
        {
            lv_led_set_color(_gridLed, lv_color_hex(0x00FF00)); // Green for online
            lv_led_on(_gridLed);                                // Turn the LED on
        }
        else
        {
            lv_led_set_color(_gridLed, lv_color_hex(0xFF0000)); // Red for offline
            lv_led_on(_gridLed);                                // Turn the LED on
        }
    }

    // Change the frame color based on power
    if (sol.feedinPower > 100)
    {
        // Dark green for power greater than +500 W
        lv_obj_set_style_border_color(_gridFrame, lv_color_hex(UIStyle::Black), 0);
        lv_obj_set_style_bg_color(_gridFrame, lv_color_hex(UIStyle::LtGreen), 0);
    }
    else if (sol.feedinPower < -100)
    {
        // Dark red for power less than -500 W
        lv_obj_set_style_border_color(_gridFrame, lv_color_hex(UIStyle::Black), 0);
        lv_obj_set_style_bg_color(_gridFrame, lv_color_hex(UIStyle::LtRed), 0);
    }
    else
    {
        // Black for power in range -500 to +500 W
        lv_obj_set_style_border_color(_gridFrame, lv_color_hex(UIStyle::Black), 0);
        lv_obj_set_style_bg_color(_gridFrame, lv_color_hex(UIStyle::White), 0);
    }
    
    // Energy bar
    if (_energyBar) 
    {
        if (sol.freeEnergy < 0)
        {
            lv_obj_set_style_bg_color(_energyBar, lv_color_hex(UIStyle::Red), LV_PART_INDICATOR); // Red for negative
        }
        else
        {
            lv_obj_set_style_bg_color(_energyBar, lv_color_hex(UIStyle::Green), LV_PART_INDICATOR); // Green for positive
        }
        lv_bar_set_value(_energyBar, abs(sol.freeEnergy ), LV_ANIM_ON);
    }
    
    if (_energyBarLabel)lv_label_set_text(_energyBarLabel, Utils::formatPower(sol.freeEnergy).c_str());  

    // HDO
    if (_hdoLed) 
    {
        if (sol.hdo)
        {
            lv_led_set_color(_hdoLed, lv_color_hex(0x00FF00)); // Green for online
            lv_led_on(_hdoLed);                                // Turn the LED on
            lv_obj_clear_flag(_hdoLed, LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_led_set_color(_hdoLed, lv_color_hex(0x00FF00)); 
            lv_led_off(_hdoLed);                                // Turn the LED OFF
            lv_obj_add_flag(_hdoLed, LV_OBJ_FLAG_HIDDEN);
        }
    }

    // Outdoor temperature
     if (_temperatureOut) lv_label_set_text(_temperatureOut, Utils::formatPower(sol.outdoorTemp,"","°C").c_str());

     // Total
    if (_totalSolLabel)  lv_label_set_text(_totalSolLabel, Utils::formatPower(sol.sol , "W", "h").c_str());
    if (_dayConsumpLabel) lv_label_set_text(_dayConsumpLabel, Utils::formatPower(sol.cons, "W", "h").c_str());   

    if (sol.errorMqtt || sol.errorWifi) 
    {
        if (sol.errorWifi)  setNowWifi();
        if (sol.errorMqtt) setNoMqtt();
    } else {
        showEnergyBar();
    }

}

  void MainScreen::updateDataSetHour(int datasetIndex, int hour, int newValue)
   {
        DDLockGuard lock;

        if (datasetIndex < 0 || datasetIndex >= 2)
        {
            ESP_LOGI(TAG, "Invalid dataset index: %d\n", datasetIndex);
            return;
        }

        if (hour < 0 || hour >= 24)
        {
            ESP_LOGI(TAG, "Invalid hour: %d\n", hour);
            return;
        }

        _dataSets[datasetIndex].data[hour] = (newValue == 0) ? LV_CHART_POINT_NONE : newValue;

        if (datasetIndex == _currentDataSetIndex && _chartSeries && _chart)
        {
            lv_chart_set_value_by_id(_chart, _chartSeries, hour, newValue);

            for (int i = hour; i < 24; i++)
            {
                lv_chart_set_value_by_id(_chart, _chartSeries, i, LV_CHART_POINT_NONE);
            }
        }

        updateChart();
        updateChartRange();
    }


 void MainScreen::updateTemperatureTextColor(lv_obj_t *label, float temp)
    {
        if (temp < 15)
        {
            lv_obj_set_style_text_color(label, lv_color_hex(UIStyle::Blue), 0); // Blue for cold
        }
        else if (temp <= 30)
        {
            lv_obj_set_style_text_color(label, lv_color_hex(UIStyle::Black), 0); // Black for normal
        }
        else
        {
            lv_obj_set_style_text_color(label, lv_color_hex(UIStyle::Red), 0); // Orange for hot
        }
    }


