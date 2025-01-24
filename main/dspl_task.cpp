
//
// vim: ts=4 et
// Copyright (c) 2023 Petr Vanek, petr@fotoventus.cz
//
/// @file   led_task.cpp
/// @author Petr Vanek

#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include "dspl_task.h"
#include "application.h"
#include "esp_log.h"
#include "key_val.h"
#include "literals.h"
#include "utils.h"

DisplayTask::DisplayTask() : _consumption("cons"), _photovoltaic("pv"), _sdcard("/sdcard", HW_SD_MOSI, HW_SD_MISO, HW_SD_CLK, HW_SD_CS)
{
    _queue = xQueueCreate(5, sizeof(DisplayTask::ReqData));
    _queueData = xQueueCreate(5, sizeof(SolaxParameters));
}

DisplayTask::~DisplayTask()
{
    done();
    if (_queue)
        vQueueDelete(_queue);

    if (_queueData)
        vQueueDelete(_queueData);
}

void DisplayTask::loop()
{
    bool mountOK = false;
    bool loadAfterReset = true;
    int lastMin = 0;
    bool firstCheck = true;

    auto *screenManager = ScreenManager::getInstance();
    screenManager->addScreen(std::make_unique<MainScreen>());
    screenManager->showScreenByType(ScreenType::Main);

    mountOK = (_sdcard.mount(true) == ESP_OK);
    if (!mountOK)
        ESP_LOGE(TAG, "SD card mount failed - memory mode");
    else
        ESP_LOGW(TAG, "SD card mode active");

    bool lastMqtt = false;
    bool lastConnection = _connectionManager ? _connectionManager->isConnected() : false;
    Application::getInstance()->signalTaskStart(Application::TaskBit::Display);

    SolarData solaxData;

    while (true)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ReqData req;
        if (xQueueReceive(_queue, &req, 0) == pdTRUE)
        {
            if (req.contnet == Contnet::UpdateData)
            {
                ESP_LOGI(TAG, "LOG  [%s]", static_cast<const char *>(req.msg));
            }
        }

        if (xQueueReceive(_queueData, &_SolaxData, 0) == pdTRUE)
        {

            solaxData.hdo = !(_SolaxData.Hdo == 0);
            solaxData.powerDC1 = _SolaxData.Powerdc1;
            solaxData.powerDC2 = _SolaxData.Powerdc2;
            solaxData.batteryCapacity = _SolaxData.BattCap;
            solaxData.batteryChargePower = _SolaxData.Batpower_Charge1;
            solaxData.batteryTemperature = _SolaxData.TemperatureBat;
            solaxData.gridPowerR = _SolaxData.GridPower_R;
            solaxData.gridPowerS = _SolaxData.GridPower_S;
            solaxData.gridPowerT = _SolaxData.GridPower_T;
            solaxData.feedinPower = _SolaxData.FeedinPower;
            solaxData.invTemp = _SolaxData.Temperature;
            solaxData.outdoorTemp = float(_SolaxData.OutTemp) / 10.0;
            solaxData.batteryCapacity = _SolaxData.BattCap;
            solaxData.batteryChargePower = _SolaxData.Batpower_Charge1;
            solaxData.batteryTemperature = _SolaxData.TemperatureBat;
            solaxData.mode = _SolaxData.RunMode;
            solaxData.onGrid = (_SolaxData.GridStatus == 0);
            auto [hour, min, sec] = Utils::getTime();

            // Update derived values
            solaxData.updateDerivedValues();

            // --------
            if (_connectionManager && _connectionManager->isTimeActive())
            {
                auto [hour, min, sec] = Utils::getTime();

                if (hour == 0 && min == 0 && lastMin != min)
                { // Day reset,
                    lastMin = min;
                    _consumption.resetDailyConsumption();
                    _photovoltaic.resetDailyConsumption();
                    auto filename = "/" + Utils::getDayFileName();
                    _sdcard.deleteFile(filename);
                    filename += ".pv";
                    _sdcard.deleteFile(filename);
                    screenManager->clearAllDataSets();
                }
                else
                {

                    if (mountOK && loadAfterReset)
                    {
                        loadAfterReset = false;
                        auto filename = "/" + Utils::getDayFileName();
                        _consumption.load(_sdcard.readFile(filename));
                        filename += ".pv";
                        _photovoltaic.load(_sdcard.readFile(filename));

                        _consumption.updateChart([this, &screenManager](int hour, float consumption)
                                                 { screenManager->updateDataSetHour(1, hour, consumption); });
                        _photovoltaic.updateChart([this, &screenManager](int hour, float consumption)
                                                  { screenManager->updateDataSetHour(0, hour, consumption); });
                    }

                    _consumption.update(solaxData.consumption);
                    _photovoltaic.update(solaxData.photovoltaic);
                    //_consumption.dump();

                    screenManager->updateDataSetHour(1, hour, _consumption.getConsumptionForHour(hour));
                    screenManager->updateDataSetHour(0, hour, _photovoltaic.getConsumptionForHour(hour));
                    ESP_LOGI(TAG, "TIME %d %d %d", hour, min, sec);

                    if (mountOK && (min % 5 == 0) && (lastMin != min))
                    {
                        lastMin = min;
                        auto filename = "/" + Utils::getDayFileName();
                        if (_sdcard.writeFile(filename, _consumption.save()))
                        {
                            ESP_LOGI(TAG, "File written successfully %s", filename.c_str());
                        }
                        filename += ".pv";
                        if (_sdcard.writeFile(filename, _photovoltaic.save()))
                        {
                            ESP_LOGI(TAG, "File written successfully %s", filename.c_str());
                        }
                    }
                }

            } // <--- valid time
            // --------

            solaxData.sol = _SolaxData.Etoday_togrid * 100;
            solaxData.cons = (int)_consumption.getSum();
        }

        // chcek connection error
        if (_connectionManager)
        {
            bool currentConnection = _connectionManager->isConnected();
            if (lastConnection != currentConnection || firstCheck)
            {
                firstCheck = false;
                lastConnection = currentConnection;
                ESP_LOGI(TAG, "Connection changed: Wifi %s", lastConnection ? "CONNECTED" : "DISCONNECTED");
                if (lastConnection)
                {
                    solaxData.errorWifi = false;
                }
                else
                {
                    solaxData.errorWifi = true;
                }
            }

            bool currentMqtt = _connectionManager->isMqttActive();
            if (lastMqtt != currentMqtt)
            {
                lastMqtt = currentMqtt;

                ESP_LOGI(TAG, "MQTT changed: %s", lastMqtt ? "ACTIVE" : "INACTIVE");
                if (!lastMqtt)
                {
                    solaxData.errorMqtt = true;
                }
                else
                {
                    solaxData.errorMqtt = false;
                }
            }
        }

        screenManager->solaxUpdate(solaxData);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    _sdcard.unmount(); // never umnounted!!!
}

void DisplayTask::settingMsg(std::string_view msg)
{

    if (_queue)
    {
        ReqData rqdt;
        rqdt.contnet = Contnet::UpdateData;

        // Ensure the message fits within the buffer, leaving room for the null-terminator
        size_t maxLength = sizeof(rqdt.msg) - 1;
        size_t copyLength = std::min(msg.size(), maxLength);
        std::memcpy(rqdt.msg, msg.data(), copyLength);
        rqdt.msg[copyLength] = '\0';
        xQueueSendToBack(_queue, &rqdt, 0);
    }
}

void DisplayTask::updateUI(const SolaxParameters &msg)
{
    if (_queueData)
    {
        xQueueSendToBack(_queueData, &msg, 0);
    }
}

bool DisplayTask::init(std::shared_ptr<ConnectionManager> connMgr, const char *name, UBaseType_t priority, const configSTACK_DEPTH_TYPE stackDepth)
{
    bool rc = false;
    _connectionManager = connMgr;
    rc = RPTask::init(name, priority, stackDepth);
    return rc;
}