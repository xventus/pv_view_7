// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   main_screen.h
/// @author Petr Vanek
///

#pragma once

#include "lvgl.h"
#include "ui/screen_if.h"
#include "sd_card.h"
#include "ui/screen_manager.h"
#include "ui/ui_style.h"

class ScreenManager;
class MainScreen : public IScreen
{
private:
    static constexpr const char *TAG = "MScr";
    lv_obj_t *_screen{nullptr};
    lv_obj_t *_solarPanelFrame{nullptr};
    lv_obj_t *_solarPanelTotalPowerLabel{nullptr};
    lv_obj_t *_solarPanelString1Label{nullptr};
    lv_obj_t *_solarPanelString2Label{nullptr};

    lv_obj_t *_batteryFrame{nullptr};
    lv_obj_t *_batteryPercentageLabel{nullptr};
    lv_obj_t *_batteryPowerLabel{nullptr};
    lv_obj_t *_batteryTempLabel{nullptr};

    lv_obj_t *_gridFrame{nullptr};
    lv_obj_t *_gridLabel{nullptr};
    lv_obj_t *_gridLed{nullptr};

    lv_obj_t *_consumptionFrame{nullptr};
    lv_obj_t *_consumptionLabel{nullptr};

    lv_obj_t *_overviewFrame{nullptr};
    lv_obj_t *_overviewPowerLabel{nullptr};
    lv_obj_t *_overviewTempLabel{nullptr};

    lv_obj_t *_l1{nullptr};
    lv_obj_t *_l2{nullptr};
    lv_obj_t *_l3{nullptr};
    lv_obj_t *_mode{nullptr};

    lv_obj_t *_energyBarFrame{nullptr};
    lv_obj_t *_energyBarLabel{nullptr};
    lv_obj_t *_energyBar{nullptr};
    lv_obj_t *_hdoLed{nullptr};

    lv_obj_t *_timePanelFrame{nullptr};
    lv_obj_t *_timeLabel{nullptr};
    lv_obj_t *_dateLabel{nullptr};

    lv_obj_t *_barGraphFrame{nullptr};
    lv_obj_t *_chartTitleLabel{nullptr};
    lv_obj_t *_chart{nullptr};
    lv_chart_series_t *_chartSeries{nullptr};

    lv_obj_t *_maxLabel{nullptr};
    lv_obj_t *_totalSolLabel{nullptr};
    lv_obj_t *_totalSol{nullptr};
    lv_obj_t *_dayConsumpLabel{nullptr};
    lv_obj_t *_totalCons{nullptr};
    int _currentDataSetIndex{0};

    lv_obj_t *_temperatureOut{nullptr};

    lv_obj_t *_messageIcon{nullptr};
    lv_obj_t *_messageLabel{nullptr};

    struct Message
    {
        const lv_img_dsc_t *icon;
        const char *text;
    };

    struct ChartDataSet
    {
        const char *description;
        lv_color_t color;
        int data[24];
    };

    ChartDataSet _dataSets[2] = {
        {"Photovoltaic Yield", lv_color_hex(UIStyle::Yellow), {LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE}},
        {"Energy Consumption", lv_color_hex(UIStyle::Red), {LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE}}};

public:
    MainScreen();

    ScreenType getType() const override;

    bool init() override;

    void down() override;

    void show() override;

    void message(std::string_view msg);
    void solaxUpdate(const SolarData &sol);
    void updateDataSetHour(int datasetIndex, int hour, int newValue);
    void clearAllDataSets();

private:
    void createFrames();
    void createBarGraph(int frameOverviewWidth, int frameOverviewHeight, int top, const char *title, lv_color_t barColor);
    void onChartClick();
    void graphDisplay(bool hideGraph);
    void updateChart();
    void updateChartRange();
    void enableOverviewClick();
    void showEnergyMessage();
    void showEnergyBar();
    void setNoMqtt();
    void setNowWifi();
    void updateEnergyMessage(const Message &message);
    void updateTemperatureTextColor(lv_obj_t *label, float temp);
  
private:
};
