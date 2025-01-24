// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   label_panel.h
/// @author Petr Vanek
///

#pragma once

#include "lvgl.h"
#include <string>
#include "ui_style.h"


/// @brief Class to create a labeled panel for displaying text with customizable styles.
class LabelPanel
{
private:
    static constexpr const char *TAG = "LabP";
    lv_obj_t *_container = nullptr;           ///< Container object for holding the label.
    lv_obj_t *_label = nullptr;               ///< Label object for displaying text.
    static constexpr lv_coord_t _margin = 10; ///< Padding inside the container.

public:
    /// @brief Constructor to initialize the LabelPanel with text and styles.
    /// @param parent The parent LVGL object.
    /// @param initialText Initial text to display on the label.
    /// @param width Width of the panel.
    /// @param height Height of the panel.
    /// @param x X-coordinate for panel alignment.
    /// @param y Y-coordinate for panel alignment.
    /// @param textSize Font size for the label text.
    LabelPanel(lv_obj_t *parent, const std::string &initialText,
               lv_coord_t width = 300, lv_coord_t height = 100,
               lv_coord_t x = 0, lv_coord_t y = 0, uint8_t textSize = 1)
    {
        createPanel(parent, initialText, width, height, x, y, textSize);
    }

    /// @brief Update the label text.
    /// @param newText New text to display on the label.
    void setText(const std::string &newText)
    {
        if (!_label)
        {
            ESP_LOGE(TAG, "Label is not initialized");
            return;
        }
        lv_label_set_text(_label, newText.c_str()); // Update text
    }

    /// @brief Get the container object.
    /// @return Pointer to the container object.
    lv_obj_t *getContainer() const
    {
        return _container;
    }

    /// @brief Get the label object.
    /// @return Pointer to the label object.
    lv_obj_t *getLabel() const
    {
        return _label;
    }

    /// @brief Set the text color of the label.
    /// @param labelTextColor Desired color for the label text.
    void setColor(lv_color_t labelTextColor)
    {
        if (_label)
        {
            lv_obj_set_style_text_color(_label, labelTextColor, 0);
            lv_obj_set_style_text_opa(_label, LV_OPA_COVER, 0);
        }
    }

    /// @brief Set the background color of the panel container.
    /// @param color Desired background color.
    /// @param opa Opacity level for the background (default: fully opaque).
    void setBackgroundColor(lv_color_t color, lv_opa_t opa = LV_OPA_COVER)
    {
        if (_container)
        {
            lv_obj_set_style_bg_color(_container, color, 0);
            lv_obj_set_style_bg_opa(_container, opa, 0);
        }
    }

private:

    /// @brief Create the panel container and label.
    /// @param parent The parent LVGL object.
    /// @param text Initial text for the label.
    /// @param width Width of the container.
    /// @param height Height of the container.
    /// @param x X-coordinate for container alignment.
    /// @param y Y-coordinate for container alignment.
    /// @param textSize Font size for the label text.
    void createPanel(lv_obj_t *parent, const std::string &text,
                     lv_coord_t width, lv_coord_t height,
                     lv_coord_t x, lv_coord_t y, uint8_t textSize)
    {
        // Create container
        _container = lv_obj_create(parent);
        lv_obj_set_size(_container, width, height);
        lv_obj_align(_container, LV_ALIGN_DEFAULT, x, y);
        lv_obj_set_style_pad_all(_container, _margin, 0);
        lv_obj_set_style_bg_color(_container, lv_color_hex(UIStyle::BackgroundContent), 0);
        lv_obj_set_style_bg_opa(_container, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(_container, 4, 0);
        lv_obj_set_style_border_color(_container, lv_color_hex(UIStyle::BorderPanel), 0);

        // Create label inside the container
        _label = lv_label_create(_container);
        if (!_label)
        {
            ESP_LOGE(TAG, "Failed to create label");
            return;
        }

        lv_label_set_text(_label, text.c_str());     // Set initial text
        lv_obj_align(_label, LV_ALIGN_CENTER, 0, 0); // Center label in the container

        switch (textSize)
        {
        case 1:
            lv_obj_set_style_text_font(_label, &lv_font_montserrat_8, 0);
            break;
        case 2:
            lv_obj_set_style_text_font(_label, &lv_font_montserrat_12, 0);
            break;
        case 3:
            lv_obj_set_style_text_font(_label, &lv_font_montserrat_16, 0);
            break;
        case 4:
            lv_obj_set_style_text_font(_label, &lv_font_montserrat_20, 0);
            break;
        case 5:
            lv_obj_set_style_text_font(_label, &lv_font_montserrat_40, 0);
            break;
        case 6:
            lv_obj_set_style_text_font(_label, &lv_font_montserrat_30, 0);
            break;
        }
    }
};
