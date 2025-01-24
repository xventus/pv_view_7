// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   button_panel.h
/// @author Petr Vanek
///

#pragma once

#include "lvgl.h"
#include <vector>
#include <string>
#include <functional>

/// @brief Struct to define the properties of each button.
struct ButtonField
{
    const char *label{nullptr};                   ///< Text label displayed on the button.
    lv_color_t textColor{lv_color_hex(0x000000)}; ///< Text color of the button (default: Black).
    lv_color_t bgColor{lv_color_hex(0xFFCCCC)};   ///< Background color of the button (default: Light red).
    std::function<void()> callback;               ///< Callback function triggered when the button is pressed.
};

/// @brief Class to create and manage a panel of buttons.
class ButtonPanel
{
private:
    static constexpr const char *TAG = "ButF";
    lv_obj_t *_container;                                ///< Main container for the button panel.
    std::vector<lv_obj_t *> _buttons;                    ///< List of button objects.
    static constexpr lv_coord_t _margin = 10;            ///< Margin between buttons.
    std::shared_ptr<std::vector<ButtonField>> _fields{}; ///< ButtonField configurations for the panel.

public:
    /// @brief Constructor for ButtonPanel.
    /// @param parent The parent LVGL object.
    /// @param fields Shared pointer to a vector of ButtonField definitions.
    /// @param width Width of the panel.
    /// @param height Height of the panel.
    /// @param x X-coordinate for panel alignment.
    /// @param y Y-coordinate for panel alignment.
    ButtonPanel(lv_obj_t *parent, std::shared_ptr<std::vector<ButtonField>> fields,
                lv_coord_t width = 300, lv_coord_t height = 400,
                lv_coord_t x = 10, lv_coord_t y = 10) : _fields(fields)
    {
        init(parent, width, height, x, y);
    }

    // Destructor
    ~ButtonPanel()
    {
        if (_container)
            lv_obj_del(_container);
    }

    /// @brief Set the background color of the panel.
    /// @param color The desired background color.
    /// @param opa The opacity level (default: fully opaque).
    void setBackgroundColor(lv_color_t color, lv_opa_t opa = LV_OPA_COVER)
    {
        if (_container)
        {
            lv_obj_set_style_bg_color(_container, color, 0);
            lv_obj_set_style_bg_opa(_container, opa, 0);
        }
    }

private:
    /// @brief Initialize the button panel.
    /// @param parent The parent LVGL object.
    /// @param width Width of the panel.
    /// @param height Height of the panel.
    /// @param x X-coordinate for panel alignment.
    /// @param y Y-coordinate for panel alignment.
    void init(lv_obj_t *parent, lv_coord_t width, lv_coord_t height, lv_coord_t x, lv_coord_t y)
    {
        // Create the container for the panel
        _container = lv_obj_create(parent);
        lv_obj_set_size(_container, width, height);
        lv_obj_align(_container, LV_ALIGN_DEFAULT, x, y);
        lv_obj_set_style_pad_all(_container, _margin, 0);
        lv_obj_set_style_bg_color(_container, lv_color_hex(UIStyle::BackgroundContent), 0);
        lv_obj_set_style_bg_opa(_container, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(_container, 4, 0);
        lv_obj_set_style_border_color(_container, lv_color_hex(UIStyle::BorderPanel), 0);

        // Create buttons based on ButtonField
        if (_fields)
        {
            for (const auto &field : *_fields)
            {
                createButton(field, width - (5 * _margin));
            }
        }
    }

    /// @brief Create an individual button.
    /// @param field The ButtonField definition for the button.
    /// @param width The width of the button.
    void createButton(const ButtonField &field, lv_coord_t width)
    {
        // Create button
        lv_obj_t *btn = lv_btn_create(_container);
        lv_obj_set_size(btn, width, 50);                                                 // Fixed height for buttons
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, _margin, _buttons.size() * (60 + _margin)); // Stack vertically

        // Set button background color
        lv_obj_set_style_bg_color(btn, field.bgColor, 0);
        lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);

        // Create label for button
        lv_obj_t *label = lv_label_create(btn);
        lv_label_set_text(label, field.label);
        lv_obj_set_style_text_color(label, field.textColor, 0); // Set text color
        lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);

        lv_obj_center(label);

        // Add callback for button
        lv_obj_add_event_cb(btn, [](lv_event_t *e)
                            {
      
            ButtonField *field = static_cast<ButtonField *>(lv_event_get_user_data(e));

            if (field && field->callback) {
                field->callback();
            } else {
                ESP_LOGE(TAG, "Callback is not set or field is null");
            } }, LV_EVENT_CLICKED, const_cast<ButtonField *>(&field)); // Pass field as user data

        // Store the button
        _buttons.push_back(btn);
    }

    /// @brief Retrieve all button objects in the panel.
    /// @return Reference to the list of button objects.
    const std::vector<lv_obj_t *> &getButtons() const
    {
        return _buttons;
    }
};
