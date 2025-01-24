// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   ui_style.h
/// @author Petr Vanek
///

#pragma once

#include "lvgl.h"

class UIStyle
{
public:
    static constexpr uint32_t BorderPanel = 0x8899FF;
    static constexpr uint32_t BackgroundContent = 0x99BBFF;
    static constexpr uint32_t BackgroundScreen = 0x99AAFF;
    static constexpr uint32_t PressedColor = 0xF0F0EE;
    static constexpr uint32_t BorderInput = 0x889FF;
    static constexpr uint32_t White = 0xFFFFFF;
    static constexpr uint32_t Blue = 0x000055;
    static constexpr uint32_t Black = 0x000000;


    static const uint32_t Green{0x00FF00};
    static const uint32_t Red{0xFF0000};
    static const uint32_t LtGreen{0xA8E6CF};
    static const uint32_t LtRed{0xFFC4C4};    
    static const uint32_t Yellow{0x005555};

    static constexpr uint32_t BorderInputW = 2;
    static constexpr uint32_t BorderPanelW = 4;
};
