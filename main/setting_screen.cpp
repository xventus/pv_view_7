// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   setting_screen.cpp
/// @author Petr Vanek
///

#include "setting_screen.h"
#include "esp_log.h"
#include "ui/screen_manager.h"
#include "ui/ui_style.h"
#include "literals.h"
#include "key_val.h"
#include "noway_screen.h"
#include "application.h"

SettingScreen::SettingScreen()
{
}

ScreenType SettingScreen::getType() const
{
    return ScreenType::Setting;
}

bool SettingScreen::init()
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

        _wifiSelector = std::make_unique<WifiSsidSelector>(_screen, 350, 200, 430, 10);
        _wifiSelector->withOnWifiSelectedCallback([this](const std::string &ssid)
                                                  { ESP_LOGI("MAIN", "********* Selected Wi-Fi: %s", ssid.c_str()); 
                                                  DDLockGuard lock;
                                                 _inputArea->setInputContent(literals::kv_ssid,ssid); });
        

        _buttonFields = std::make_shared<std::vector<ButtonField>>(
            std::initializer_list<ButtonField>{
                {"Apply changes & Restart", lv_color_hex(0xFFFFFF), lv_color_hex(UIStyle::Blue), [this]()
                 {
                    saveSetting();
                    Application::getInstance()->getResetTask()->reset();
                    while (true) {
                        vTaskDelay(100 / portTICK_PERIOD_MS);
                    }
                 }},
                {"Run WiFi AP for setup", lv_color_hex(0x000000), lv_color_hex(UIStyle::Green), [this]()
                 {
                    _wifiSelector->stopPeriodicScan();
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                     ScreenManager::getInstance()->addScreen(std::make_unique<NoWayScreen>());
                     ScreenManager::getInstance()->showScreenByType(ScreenType::NoWay);

                     Application::getInstance()->getWifiTask()->setPriority(tskIDLE_PRIORITY + 3ul);
                     Application::getInstance()->getWifiTask()->switchMode(WifiTask::Mode::AP);
                  

                 }},
                {"Reboot", lv_color_hex(0xFFFFFF), lv_color_hex(UIStyle::Red), [this]()
                 {
                     Application::getInstance()->getResetTask()->reset();
                      while (true) {
                        vTaskDelay(100 / portTICK_PERIOD_MS);
                    }
                 }}});

        _buttonPanel = std::make_unique<ButtonPanel>(_screen, _buttonFields, 350, 230, 430, 230);

       
        KeyVal& kv = KeyVal::getInstance();		

        std::vector<InputField> fields = {
            {literals::kv_ssid, "SSID:", kv.readString(literals::kv_ssid,"myAP"), 20, lv_color_hex(UIStyle::LtRed), false},
            {literals::kv_passwd, "Password:", kv.readString(literals::kv_passwd,""), 20, lv_color_hex(UIStyle::LtRed), true},
            {literals::kv_mqtt, "Mqtt broker:", kv.readString(literals::kv_mqtt,"192.168.1.20"), 30, lv_color_hex(UIStyle::LtRed), false},
            {literals::kv_topic, "Mqtt topic:", kv.readString(literals::kv_topic,"solax/data"), 30, lv_color_hex(UIStyle::LtRed), false},
            { literals::kv_timezone, "Timezone:", kv.readString(literals::kv_timezone,literals::kv_def_timezone), 30, lv_color_hex(UIStyle::White), false},
            {literals::kv_timeserver, "Time servert (NTP):", kv.readString(literals::kv_timeserver,literals::kv_def_timeserver), 30, lv_color_hex(UIStyle::White), false},
            {literals::kv_ip, "IP:", kv.readString(literals::kv_ip,""), 20, lv_color_hex(UIStyle::White), false},
            {literals::kv_mask, "Mask:", kv.readString(literals::kv_mask,""), 40, lv_color_hex(UIStyle::White), false},
            {literals::kv_gtw, "Gateway:", kv.readString(literals::kv_gtw,""), 20, lv_color_hex(UIStyle::White), false},
            {literals::kv_dns, "Dns:", kv.readString(literals::kv_dns,""), 30, lv_color_hex(UIStyle::White), false},   
            {literals::kv_user, "Mqtt user:", kv.readString(literals::kv_user,""), 30, lv_color_hex(UIStyle::White), false},
            {literals::kv_passwdbr, "Mqtt password:", kv.readString(literals::kv_passwdbr,""), 30, lv_color_hex(UIStyle::White), false} 
            };

        _inputArea = std::make_unique<ScrollableInputArea>(_screen, fields, 200, 400, 450, 10, 10);

        rc = true;
    } while (false);
    return rc;
}

void SettingScreen::saveSetting()
{
    KeyVal& kv = KeyVal::getInstance();
    

    kv.writeString(literals::kv_passwd, _inputArea->getInputContent(literals::kv_passwd));
    kv.writeString(literals::kv_ip, _inputArea->getInputContent(literals::kv_ip));
    kv.writeString(literals::kv_mask, _inputArea->getInputContent(literals::kv_mask));
    kv.writeString(literals::kv_gtw, _inputArea->getInputContent(literals::kv_gtw));
    kv.writeString(literals::kv_ssid, _inputArea->getInputContent(literals::kv_ssid));
    kv.writeString(literals::kv_dns, _inputArea->getInputContent(literals::kv_dns));
    kv.writeString(literals::kv_mqtt, _inputArea->getInputContent(literals::kv_mqtt));
    kv.writeString(literals::kv_user, _inputArea->getInputContent(literals::kv_user));
    kv.writeString(literals::kv_passwdbr, _inputArea->getInputContent(literals::kv_passwdbr));
    kv.writeString(literals::kv_topic, _inputArea->getInputContent(literals::kv_topic));
    kv.writeString(literals::kv_timezone, _inputArea->getInputContent(literals::kv_timezone));
    kv.writeString(literals::kv_timeserver, _inputArea->getInputContent(literals::kv_timeserver));
}

void SettingScreen::down()
{
    if (_screen && lv_obj_is_valid(_screen))
    {
        lv_obj_del(_screen);
        _screen = nullptr;
    }

    _wifiSelector.reset();
    _inputArea.reset();
    _buttonPanel.reset();
    _buttonFields.reset();

    ESP_LOGI(TAG, "SettingScreen resources released.");
}

void SettingScreen::show()
{
    if (_screen)
    {
        DDLockGuard lock;
        lv_scr_load(_screen);
        if (!_started) 
        {
             auto cm = ScreenManager::getInstance()->getConnectionManager();
             ESP_LOGI(TAG, "Check conneciton manager isXXX");
             while (cm->isConnected() || cm->isAPActive()) {
                 vTaskDelay(100 / portTICK_PERIOD_MS);
             }

            vTaskDelay(1000 / portTICK_PERIOD_MS);
            ESP_LOGI(TAG, "Start scanner");
           
            _wifiSelector->startPeriodicScan();
            _started = true;
        }
    }
}



void SettingScreen::changeBackgroud()
{
    DDLockGuard lock;
   
    if (_buttonPanel)
        _buttonPanel->setBackgroundColor(_backgroudTog ? lv_color_hex(UIStyle::White) : lv_color_hex(UIStyle::BackgroundContent));
    if (_wifiSelector)
        _wifiSelector->setBackgroundColor(_backgroudTog ? lv_color_hex(UIStyle::White) : lv_color_hex(UIStyle::BackgroundContent));
    if (_inputArea)
        _inputArea->setBackgroundColor(_backgroudTog ? lv_color_hex(UIStyle::White) : lv_color_hex(UIStyle::BackgroundContent));

    _backgroudTog = !_backgroudTog;
}