
// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   ui.h
/// @author Petr Vanek
///

#pragma once

#include "lvgl.h"
#include "ui_style.h"

class UI
{
    public:

    static lv_obj_t *addIcon(lv_obj_t *parent, const lv_img_dsc_t *icon, lv_align_t align, int x, int y)
    {
        lv_obj_t *img = lv_img_create(parent);
        lv_img_set_src(img, icon);
        lv_obj_align(img, align, x, y);
        return img;
    }


    static lv_obj_t *addLabel(lv_obj_t *parent, const char *text, lv_align_t align, int x, int y, const lv_font_t *font)
    {
        lv_obj_t *label = lv_label_create(parent);
        lv_label_set_text(label, text);
        lv_obj_set_style_text_font(label, font, 0);
        lv_obj_set_style_text_color(label, lv_color_black(), 0);
        lv_obj_align(label, align, x, y);
        return label;
    }


    static lv_obj_t *createFrame(lv_obj_t * screen, int width, int height, lv_align_t align, int x, int y, lv_color_t borderColor, lv_color_t bgColor)
    {
        lv_obj_t *frame = lv_obj_create(screen);
        lv_obj_set_size(frame, width, height);
        lv_obj_align(frame, align, x, y);

        // Frame style
        lv_obj_set_style_radius(frame, 10, 0);
        lv_obj_set_style_border_width(frame, 4, 0);
        lv_obj_set_style_border_color(frame, borderColor, 0);
        lv_obj_set_style_bg_color(frame, bgColor, 0);
        lv_obj_set_style_bg_opa(frame, LV_OPA_COVER, 0);

        return frame;
    }

   

    static lv_obj_t *addTemperatureLabel(lv_obj_t *parent, const char *text, lv_align_t align, int x, int y)
    {
        lv_obj_t *label = lv_label_create(parent);
        lv_label_set_text(label, text);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);
        updateTemperatureTextColor(label, atof(text));
        lv_obj_align(label, align, x, y);
        return label;
    }

    static void updateTemperatureTextColor(lv_obj_t *label, float temp)
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

    static  lv_obj_t *addBar(lv_obj_t *parent, int width, int height, lv_align_t align, int x, int y)
    {
        lv_obj_t *bar = lv_bar_create(parent);
        lv_obj_set_size(bar, width, height);
        lv_obj_align(bar, align, x, y);
        lv_bar_set_range(bar, 0, 5000);
        lv_bar_set_value(bar, 0, LV_ANIM_OFF);
        return bar;
    }




};