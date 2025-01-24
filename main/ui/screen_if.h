// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   screen_if.h
/// @author Petr Vanek
///

#pragma once

#include <memory>

/**
 * @enum ScreenType
 * @brief Enum representing the different types of screens.
 * 
 * This enum is used to identify and differentiate between various screen types
 * managed by the `ScreenManager`.
 */
enum class ScreenType {
    Setting, ///< Represents a settings screen.
    Main,    ///< Represents the main screen.
    NoWay,  ///< Represents the no way screen.
    Unknown  ///< Represents an unknown or undefined screen type.
};

/**
 * @class IScreen
 * @brief Abstract interface for screens.
 * 
 * This interface defines the common functionality that all screens must implement.
 * It is intended to be implemented by specific screen types to standardize their
 * behavior within the `ScreenManager`.
 */
class IScreen {
public:
    /**
     * @brief Virtual destructor.
     * 
     * Ensures proper cleanup of derived classes.
     */
    virtual ~IScreen() = default;

    /**
     * @brief Initializes the screen.
     * 
     * This method should contain any setup logic required for the screen, such as
     * creating UI elements or preparing resources.
     * 
     * @return True if the initialization was successful, false otherwise.
     */
    virtual bool init() = 0;

    /**
     * @brief Freeup the screen.
     * 
     */
    virtual void down() = 0;

    /**
     * @brief Displays the screen.
     * 
     *  This method should load the screen's content onto the display.
     * 
     */
    virtual void show() = 0;

    /**
     * @brief Gets the type of the screen.
     * 
     * This method should return the specific type of the screen, as defined by the
     * `ScreenType` enum.
     * 
     * @return The type of the screen.
     */
    virtual ScreenType getType() const = 0;
};
