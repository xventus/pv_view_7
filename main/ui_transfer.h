// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   ui_transfer.h
/// @author Petr Vanek
///

#pragma once

/// @brief Structure to store and transfer solar energy data.
struct SolarData
{
    // HDO data
    bool hdo{false}; ///< Indicates HDO =.

    // Solar panels
    float powerDC1{0}; ///< Power output of the first solar panel (DC).
    float powerDC2{0}; ///< Power output of the second solar panel (DC).

    // Battery
    int batteryCapacity{0};      ///< Battery capacity in percentage (%).
    float batteryChargePower{0}; ///< Power used to charge the battery.
    float batteryTemperature{0}; ///< Temperature of the battery in degrees Celsius (°C).

    // Inverter
    float gridPowerR{0};    ///< Power output on the R phase.
    float gridPowerS{0};    ///< Power output on the S phase.
    float gridPowerT{0};    ///< Power output on the T phase.
    float feedinPower{0};   ///< Power fed back into the grid.
    float invTemp{0};       ///< nverter temperature
    float inverterTotal{0}; ///< total

    float outdoorTemp{0}; ///< Outdoor temperature

    // Derived values (calculated from the primary data)
    float consumption{0};  ///< Total energy consumption.
    float photovoltaic{0}; ///< Total photovoltaic power generated.
    float freeEnergy{0};   ///< Free energy available after consumption.
    int sol{0};            ///< PVE
    int cons{0};           ///< Consumption
    int mode{0};           ///< Working mode
    bool onGrid{false};    ///< Grid Status
    bool errorWifi{false}; ///< Wifi not connected
    bool errorMqtt{false}; ///< MQTT error

    /// @brief Updates the derived values based on primary data.
    void updateDerivedValues()
    {
        // Calculate the total power from the inverter
        inverterTotal = gridPowerR + gridPowerS + gridPowerT;
        // Calculate the total energy consumption
        consumption = inverterTotal - feedinPower;
        // Calculate the total power generated by solar panels
        photovoltaic = powerDC1 + powerDC2;
        // Calculate free energy available
        freeEnergy = photovoltaic - consumption;
    }
};
