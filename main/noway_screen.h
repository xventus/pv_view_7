

// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   noway_screen.h
/// @author Petr Vanek
///

#pragma once

#include "ui/screen_manager.h"
#include "ui/scrool_input_area.h"
#include "ui/wifi_selector.h"
#include "ui/button_panel.h"
#include "ui/label_panel.h"

class NoWayScreen : public IScreen
{
private:
    static constexpr const char *TAG = "NWS";
    lv_obj_t *_screen{nullptr};
    std::unique_ptr<ButtonPanel> _buttonPanel;
    std::unique_ptr<LabelPanel> _labelPanel;
    std::shared_ptr<std::vector<ButtonField>> _buttonFields;

public:
    ScreenType getType() const override;

    bool init() override;

    void down() override;

    void show() override;
};
