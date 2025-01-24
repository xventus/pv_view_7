

// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   screen_manager.cpp
/// @author Petr Vanek
///

#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>
#include "screen_manager.h"
#include "setting_screen.h"
#include "main_screen.h"
#include "esp_log.h"

// Singleton instance getter
ScreenManager *ScreenManager::getInstance()
{
    static ScreenManager instance; // Thread-safe static initialization
    return &instance;
}

// Overloaded -> operator
ScreenManager *ScreenManager::operator->()
{
    return ScreenManager::getInstance();
}

ScreenManager *ScreenManager::operator->() const
{
    return ScreenManager::getInstance();
}

void ScreenManager::initLCD(bool needInitI2C, i2c_port_t i2cPort)
{
    _dd.initBus(needInitI2C, i2cPort);
    _dd.start();
}

std::optional<size_t> ScreenManager::addScreen(std::unique_ptr<IScreen> screen)
{
    if (!screen)
    {
        ESP_LOGE(TAG, "Invalid screen pointer");
        return std::nullopt;
    }

    if (!screen->init())
    {
        ESP_LOGE(TAG, "Failed to initialize screen");
        return std::nullopt;
    }

    const auto tag = screen->getType();
    if (_screenIndexByName.find(tag) != _screenIndexByName.end())
    {
        ESP_LOGE(TAG, "Screen already exists");
        return std::nullopt;
    }

    size_t index = _screens.size();
    _screenIndexByName[tag] = index;
    _screens.push_back(std::move(screen));
    return index;
}

void ScreenManager::lock()
{
    _dd.lock();
}

void ScreenManager::unlock()
{
    _dd.unlock();
}

bool ScreenManager::showScreen(std::size_t index)
{
    if (index >= _screens.size() || !_screens[index])
    {
        ESP_LOGE(TAG, "Invalid screen index: %zu", index);
        return false;
    }

    {
        _screens[index]->show();
    }

    _currentScreen = _screens[index].get();
    return true;
}

bool ScreenManager::showScreenByType(ScreenType tag)
{
    auto rc = false;
    auto it = _screenIndexByName.find(tag);

    do
    {
        if (it == _screenIndexByName.end())
        {
            ESP_LOGE(TAG, "No screen found ");
            break;
        }
               
        rc = showScreen(it->second);

    } while (false);

    return rc;
}

void ScreenManager::removeScreenByType(ScreenType tag)
{
    auto it = _screenIndexByName.find(tag);
    if (it == _screenIndexByName.end())
    {
        ESP_LOGE(TAG, "No screen found for type");
        return;
    }

    size_t index = it->second;

    if (_currentScreen == _screens[index].get())
    {
        for (size_t i = 0; i < _screens.size(); ++i)
        {
            if (_screens[i] && i != index)
            {
                showScreen(i);
                break;
            }
        }
    }

    if (_screens[index])
    {
        _screens[index]->down();
        _screens[index].reset();
    }

    _screenIndexByName.erase(it);
    ESP_LOGI(TAG, "Screen removed by type: %d", static_cast<int>(tag));
}

void ScreenManager::removeScreenByIndex(size_t index)
{
    if (index >= _screens.size() || !_screens[index])
    {
        ESP_LOGE(TAG, "Invalid screen index: %zu", index);
        return;
    }

    if (_currentScreen == _screens[index].get())
    {
        for (size_t i = 0; i < _screens.size(); ++i)
        {
            if (_screens[i] && i != index)
            {
                showScreen(i);
                break;
            }
        }
    }

    if (_screens[index])
    {
        _screens[index]->down();
        _screens[index].reset();
    }

    for (auto it = _screenIndexByName.begin(); it != _screenIndexByName.end(); ++it)
    {
        if (it->second == index)
        {
            _screenIndexByName.erase(it);
            break;
        }
    }

    ESP_LOGI(TAG, "Screen removed by index: %zu", index);
}


void ScreenManager::solaxUpdate(const SolarData &sol)
{
    auto it = _screenIndexByName.find(ScreenType::Main);
    if (it == _screenIndexByName.end())
    {
        ESP_LOGE(TAG, "solaxUpdate - Main not found");
        return;
    }

    auto &screen = _screens[it->second];
    if (screen->getType() == ScreenType::Main)
    {
        // RTTI disabled dynamic_cast not allowed
        static_cast<MainScreen *>(screen.get())->solaxUpdate(sol);
    }
}

void ScreenManager::clearAllDataSets()
{
    auto it = _screenIndexByName.find(ScreenType::Main);
    if (it == _screenIndexByName.end())
    {
        ESP_LOGE(TAG, "clearAllDataSets - Main not found");
        return;
    }

    auto &screen = _screens[it->second];
    if (screen->getType() == ScreenType::Main)
    {
        // RTTI disabled dynamic_cast not allowed
        static_cast<MainScreen *>(screen.get())->clearAllDataSets();
    }
}


void ScreenManager::updateDataSetHour(int datasetIndex, int hour, int newValue)
{
    auto it = _screenIndexByName.find(ScreenType::Main);
    if (it == _screenIndexByName.end())
    {
        ESP_LOGE(TAG, "clearAllDataSets - Main not found");
        return;
    }

    auto &screen = _screens[it->second];
    if (screen->getType() == ScreenType::Main)
    {
        // RTTI disabled dynamic_cast not allowed
        static_cast<MainScreen *>(screen.get())->updateDataSetHour(datasetIndex, hour, newValue);
    }
}


