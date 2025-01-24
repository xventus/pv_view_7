

// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   scrool_input_area.h
/// @author Petr Vanek
///

#pragma once

#include "lvgl.h"
#include <vector>
#include <string>
#include "ui_style.h"

/// @brief Struct for defining the properties of individual input fields.
struct InputField
{
    const char *id{nullptr};                    ///< Unique identifier for the field.
    const char *label{nullptr};                 ///< Label for the input field.
    std::string content{nullptr};               ///< Initial content of the field.
    int32_t maxchar{20};                        ///< Maximum number of input characters.
    lv_color_t bgColor{lv_color_hex(0xFFCCCC)}; ///< Background color of the input field.
    bool passwd{false};                         ///< Enables password input mode if true.
};

/// @brief Class to create a scrollable area containing multiple input fields and a shared keyboard.
class ScrollableInputArea
{
private:
    static constexpr const char *TAG = "ScrlInp"; ///< Logging tag.
    lv_obj_t *_container{nullptr};                ///< Scrollable container for input fields.
    lv_obj_t *_keyboard{nullptr};                 ///< Shared keyboard object for text input.
    std::vector<lv_obj_t *> _textAreas;           ///< List of text area objects corresponding to input fields.
    std::vector<InputField> _fields;              ///< Configuration for input fields.
    lv_coord_t _keyboardHeight;                   ///< Height of the keyboard.
    static constexpr lv_coord_t _margin = 10;     ///< Margin between UI elements.

public:
    /// @brief Constructor to initialize the ScrollableInputArea.
    /// @param parent The parent LVGL object.
    /// @param fields A vector of InputField definitions for configuring input fields.
    /// @param keyboardHeight The height of the keyboard.
    /// @param width The width of the scrollable container.
    /// @param height The height of the scrollable container.
    /// @param x X-coordinate for positioning the container.
    /// @param y Y-coordinate for positioning the container.
    ScrollableInputArea(lv_obj_t *parent, const std::vector<InputField> &fields, lv_coord_t keyboardHeight = 200, lv_coord_t width = 400, lv_coord_t height = 400, lv_coord_t x = 10, lv_coord_t y = 10)
        : _container(nullptr), _keyboard(nullptr), _fields(fields), _keyboardHeight(keyboardHeight)
    {
        init(parent, width, height, x, y);
    }

    /// @brief Destructor to clean up resources.
    ~ScrollableInputArea()
    {
        if (_container)
            lv_obj_del(_container);
        if (_keyboard)
            lv_obj_del(_keyboard);
    }

    /// @brief Get the content of a specific input field by its ID.
    /// @param id The unique identifier of the input field.
    /// @return The content of the input field as a string.
    std::string getInputContent(const char *id) const
    {
        for (size_t i = 0; i < _fields.size(); ++i)
        {
            if (std::string(_fields[i].id) == id)
            {
                return lv_textarea_get_text(_textAreas[i]);
            }
        }
        return "";
    }

    /// @brief Set the content of a specific input field by its ID.
    /// @param id The unique identifier of the input field.
    /// @param content The new content to set.
    void setInputContent(const char *id, const std::string &content)
    {
        for (size_t i = 0; i < _fields.size(); ++i)
        {
            if (std::string(_fields[i].id) == id)
            {
                lv_textarea_set_text(_textAreas[i], content.c_str());
                _fields[i].content = content; // Update the internal field content
                break;
            }
        }
    }

    /// @brief Set the background color of the scrollable container.
    /// @param color The desired background color.
    /// @param opa The opacity level (default: fully opaque).
    void setBackgroundColor(lv_color_t color, lv_opa_t opa = LV_OPA_COVER)
    {
        if (_container)
        {
            lv_obj_set_style_bg_color(_container, color, 0); // Set the new background color
            lv_obj_set_style_bg_opa(_container, opa, 0);     // Set the new opacity
        }
    }

private:
    /// @brief Initialize the scrollable input area and its components.
    void init(lv_obj_t *parent, lv_coord_t width, lv_coord_t height, lv_coord_t x, lv_coord_t y)
    {
        // Create a scrollable container
        _container = lv_obj_create(parent);
        lv_obj_set_size(_container, width, height);
        lv_obj_set_style_pad_all(_container, _margin, 0);
        lv_obj_set_scroll_dir(_container, LV_DIR_VER);
        lv_obj_align(_container, LV_ALIGN_DEFAULT, x, y);
        lv_obj_set_style_bg_color(_container, lv_color_hex(UIStyle::BackgroundContent), 0);
        lv_obj_set_style_bg_opa(_container, LV_OPA_COVER, 0);

        lv_obj_set_style_border_width(_container, 4, 0); // 4 pixels wide

        // Set the border color
        lv_obj_set_style_border_color(_container, lv_color_hex(UIStyle::BorderPanel), 0);

        // Set the border opacity
        lv_obj_set_style_border_opa(_container, LV_OPA_COVER, 0); // Fully opaque

        // Create a shared keyboard
        _keyboard = lv_keyboard_create(parent);
        lv_obj_set_size(_keyboard, LV_HOR_RES, _keyboardHeight);
        lv_obj_align(_keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_add_flag(_keyboard, LV_OBJ_FLAG_HIDDEN);

        lv_obj_add_event_cb(_keyboard, [](lv_event_t *e)
                            {
                                lv_event_code_t code = lv_event_get_code(e);
                                lv_obj_t *keyboard = lv_event_get_target(e);

                                if (code == LV_EVENT_READY)
                                {
                                    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
                                    lv_obj_t *textarea = lv_keyboard_get_textarea(keyboard);
                                    if (textarea)
                                    {
                                        const char *text = lv_textarea_get_text(textarea);
                                    }
                                } }, LV_EVENT_ALL, nullptr);

        for (const auto &field : _fields)
        {
            createInputField(field, width);
        }
    }

    /// @brief Create an individual input field.// Create an individual input field
    void createInputField(const InputField &field, lv_coord_t width)
    {
        // Create label
        lv_obj_t *label = lv_label_create(_container);
        lv_label_set_text(label, field.label);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 2 * _margin, _textAreas.size() * 70);

        // Create input text area
        lv_obj_t *textarea = lv_textarea_create(_container);
        lv_textarea_set_text(textarea, field.content.c_str());
        lv_obj_set_size(textarea, width - (5 * _margin), 40);
        lv_obj_align(textarea, LV_ALIGN_TOP_LEFT, _margin, _textAreas.size() * 70 + 20);

        // Set text area properties
        lv_obj_set_style_bg_color(textarea, field.bgColor, 0);
        lv_textarea_set_one_line(textarea, true);
        lv_textarea_set_max_length(textarea, field.maxchar);

        // Enable password mode if necessary
        if (field.passwd)
        {
            lv_textarea_set_password_mode(textarea, true);      // Enable password mode
            lv_textarea_set_password_show_time(textarea, 2000); // Optional: Show characters for 2 seconds after typing
        }

        lv_obj_set_style_border_color(textarea, lv_color_hex(UIStyle::BorderInput), 0);
        lv_obj_set_style_border_width(textarea, UIStyle::BorderInputW, 0);
        lv_obj_set_style_border_opa(textarea, LV_OPA_COVER, 0);

        lv_obj_add_event_cb(textarea, [](lv_event_t *e)
                            {
    lv_obj_t *textarea = lv_event_get_target(e);
    lv_obj_t *keyboard = static_cast<lv_obj_t *>(lv_event_get_user_data(e));
    lv_obj_t *scrollable = lv_obj_get_parent(textarea);

     if (lv_event_get_code(e) == LV_EVENT_FOCUSED) {

        lv_keyboard_set_textarea(keyboard, textarea);
        lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

        lv_area_t textarea_coords, keyboard_coords;
        lv_obj_get_coords(textarea, &textarea_coords);
        lv_obj_get_coords(keyboard, &keyboard_coords);

        if (textarea_coords.y2 > keyboard_coords.y1) {
            lv_coord_t offset = - ((textarea_coords.y2 - keyboard_coords.y1) + _margin); 
            lv_obj_scroll_by(scrollable, 0, offset, LV_ANIM_ON);
        }
    } else if (lv_event_get_code(e) == LV_EVENT_DEFOCUSED) {
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
        lv_obj_scroll_to(scrollable, LV_COORD_MIN, 0, LV_ANIM_ON);
    } }, LV_EVENT_ALL, _keyboard);

        _textAreas.push_back(textarea);
    }
};
