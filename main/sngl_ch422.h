
// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   sngl_ch422.h
/// @author Petr Vanek
///


#pragma once

#include "CH422G.h"
#include "esp_log.h"

/**
 * @class DisplayEXT7
 * @brief Singleton for project-specific use of CH422G.
 */
class DisplayEXT7 : public CH422G {
private:
    
   // Private constructor for singleton
    DisplayEXT7(i2c_port_t i2cPort)
        : CH422G(i2cPort) {
             ESP_LOGW("DisplayEXT7", "Singleton CH422G active");
        }

     // Delete copy constructor and assignment operator.
    DisplayEXT7(const DisplayEXT7 &) = delete;
    DisplayEXT7 &operator=(const DisplayEXT7 &) = delete;

public:

    /**
     * @brief Gets the singleton instance of DisplayEXT7.
     * @return Pointer to the DisplayEXT7 instance.
     */
    static DisplayEXT7 *getInstance();


     /**
     * @brief Overloaded operator to access singleton methods directly.
     * @return Pointer to the ProjectCH422G instance.
     */
    DisplayEXT7 *operator->();
    DisplayEXT7 *operator->() const;

};
