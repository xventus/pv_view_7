// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   noway_screen.cpp
/// @author Petr Vanek
///

#include "noway_screen.h"
#include "esp_log.h"
#include "ui/screen_manager.h"
#include "ui/ui.h"
#include "utils.h"
#include "ui/icons8_wifi_24.h"
#include "application.h"

ScreenType NoWayScreen::getType() const
{
    return ScreenType::NoWay;
}

bool NoWayScreen::init()
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

        std::string st = "     ";
        st += literals::ap_name;
        st += " \n \n http://192.168.4.1";
        _labelPanel = std::make_unique<LabelPanel>(_screen, st.c_str(), 600, 200, 100, 80, 4);

        _buttonFields = std::make_shared<std::vector<ButtonField>>(
            std::initializer_list<ButtonField>{
                {"Restart", lv_color_hex(0xFFFFFF), lv_color_hex(UIStyle::Red), [this]()
                 {
                     Application::getInstance()->getResetTask()->reset();
                 }}});

        _buttonPanel = std::make_unique<ButtonPanel>(_screen, _buttonFields, 600, 80, 100, 350);

        rc = true;
    } while (false);
    return rc;
}

void NoWayScreen::show()
{
    if (_screen)
    {
        DDLockGuard lock;
        lv_scr_load(_screen);
    }
}

void NoWayScreen::down()
{
    if (_screen && lv_obj_is_valid(_screen))
    {
        lv_obj_del(_screen);
        _screen = nullptr;
    }

    _buttonPanel.reset();
    _labelPanel.reset();

    ESP_LOGI(TAG, "NoWayScreen resources released.");
}
