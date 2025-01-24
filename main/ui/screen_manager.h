

// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   screen_manager.h
/// @author Petr Vanek
///

#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>
#include "screen_if.h"
#include "display_driver.h"
#include "ui_transfer.h"
#include "../connection_manager.h"

/**
 * @class ScreenManager
 * @brief Manages screens and their interactions with the display driver.
 *
 * Singleton class to manage screen lifecycle, switching, and specific operations
 * like updating text or LED state.
 */
class ScreenManager
{
private:
    static constexpr const char *TAG = "ScreenManager"; ///< Logging tag.

    std::vector<std::unique_ptr<IScreen>> _screens;                 ///< Managed screens.
    std::unordered_map<ScreenType, std::size_t> _screenIndexByName; ///< Screen type to index map.
    IScreen *_currentScreen = nullptr;                              ///< Currently active screen.
    DisplayDriver _dd;                                              ///< Display driver for LCD control.
    std::shared_ptr<ConnectionManager> _connectionManager{};          ///< Connection manager

    /**
     * @brief Private constructor for singleton.
     */
    ScreenManager() = default;

    // Delete copy constructor and assignment operator.
    ScreenManager(const ScreenManager &) = delete;
    ScreenManager &operator=(const ScreenManager &) = delete;
    
public:
    /**
     * @brief Get the singleton instance.
     * @return Pointer to the ScreenManager instance.
     */
    static ScreenManager *getInstance();

    /**
     * @brief Overloaded operator to access singleton methods directly.
     * @return Pointer to the ScreenManager instance.
     */
    ScreenManager *operator->();
    ScreenManager *operator->() const;

    /**
     * @brief Initializes the LCD driver. A call is needed before the display is used for the first time !!!
     */
    void initLCD(bool needInitI2C, i2c_port_t i2cPort);

    /**
     * @brief Adds a new screen to the manager.
     * @param screen Unique pointer to the screen to add.
     * @return The index of the added screen, or std::nullopt if it fails.
     */
    std::optional<size_t> addScreen(std::unique_ptr<IScreen> screen);

    /**
     * @brief Locks the manager for thread-safe operations.
     */
    void lock();

    /**
     * @brief Unlocks the manager after thread-safe operations.
     */
    void unlock();

    /**
     * @brief Displays a screen by its index.
     * @param index Index of the screen to display.
     * @return True if successful, false otherwise.
     */
    bool showScreen(std::size_t index);

    /**
     * @brief Displays a screen by its type.
     * @param tag Type of the screen to display.
     * @return True if successful, false otherwise.
     */
    bool showScreenByType(ScreenType tag);

    void removeScreenByType(ScreenType tag);
    void removeScreenByIndex(size_t index);

    void withConnectionMnager(std::shared_ptr<ConnectionManager> connectionManager) {_connectionManager = connectionManager; }
    std::shared_ptr<ConnectionManager> getConnectionManager() { return _connectionManager; } 

     void solaxUpdate(const SolarData &sol);
     void clearAllDataSets();
    void updateDataSetHour(int datasetIndex, int hour, int newValue);

};

/**
 * @class Display Driver LockGuard
 * @brief Provides RAII-style locking and unlocking of the Display Driver via Screen Manager.
 *
 * LockGuard ensures that the ScreenManager is locked when the guard is created
 * and automatically unlocked when the guard is destroyed. This provides a safe
 * and convenient way to manage locking and unlocking in critical sections.
 */
class DDLockGuard
{
private:
    static constexpr const char *TAG = "LockGuard"; ///< Tag used for logging.

public:
    /**
     * @brief Constructs a LockGuard and locks the manager.
     *
     * @param manager Reference to the ScreenManager to lock.
     */
    explicit DDLockGuard()
    {
        //ESP_LOGW(TAG, "LOCK ---->");
        ScreenManager::getInstance()->lock();
    }

    /**
     * @brief Destroys the LockGuard and unlocks the manager.
     */
    ~DDLockGuard()
    {
        //ESP_LOGW(TAG, "UNLOCK <---");
        ScreenManager::getInstance()->unlock();
    }
};
