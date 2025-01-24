
// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   sngl_ch422.h
/// @author Petr Vanek
///

/// Singleton for WaveShare expander 

#include "sngl_ch422.h"
#include "hardware.h"


DisplayEXT7 *DisplayEXT7::getInstance()
{
    static DisplayEXT7 instance(HW_I2C_NUM); // Thread-safe static initialization
    return &instance;
}

// Overloaded -> operator
DisplayEXT7 *DisplayEXT7::operator->()
{
    return DisplayEXT7::getInstance();
}

DisplayEXT7 *DisplayEXT7::operator->() const
{
    return DisplayEXT7::getInstance();
}