//
// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   sd_card.h
/// @author Petr Vanek

#pragma once

#include <iostream>
#include <dirent.h>
#include <cstdio>
#include <cstring>
#include <vector>
#include "hardware.h"
#include <esp_vfs_fat.h>
#include <sdmmc_cmd.h>
#include <driver/sdspi_host.h>
#include <freertos/semphr.h>
#include "sngl_ch422.h"

class SdCard
{
private:
    static constexpr const char *TAG = "SdCard";
    std::string _mountPoint;
    sdmmc_card_t *_sdCard;
    int _mosi, _miso, _clk, _cs;
    SemaphoreHandle_t _mutex;

public:
    SdCard(const std::string &mountPoint, int mosi, int miso, int clk, int cs)
        : _mountPoint(mountPoint), _sdCard(nullptr), _mosi(mosi), _miso(miso), _clk(clk), _cs(cs)
    {
        _mutex = xSemaphoreCreateMutex();
    }

    ~SdCard()
    {
        if (_sdCard)  unmount();
        if (_mutex)
        {
            vSemaphoreDelete(_mutex);
        }
    }

    esp_err_t mount(bool failOnFormat)
    {
        esp_err_t ret;

        const esp_vfs_fat_mount_config_t mountConfig = {
            .format_if_mount_failed = failOnFormat,
            .max_files = 5,
            .allocation_unit_size = 16 * 1024,
            .disk_status_check_enable = false,
            .use_one_fat = false};

        const spi_bus_config_t busCfg = {
            .mosi_io_num = _mosi,
            .miso_io_num = _miso,
            .sclk_io_num = _clk,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 4000};

        if (spi_bus_free(SPI2_HOST) == ESP_OK)
        {
            ESP_LOGI(TAG, "SPI bus was already initialized. Freed before reinitializing.");
        }

        ret = spi_bus_initialize(SPI2_HOST, &busCfg, SPI_DMA_CH_AUTO);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
            return ret;
        }

        DisplayEXT7::getInstance()->setOutput(HW_EX_SD_CS, false);
        esp_rom_delay_us(100 * 1000);

        sdspi_device_config_t slotConfig = SDSPI_DEVICE_CONFIG_DEFAULT();
        slotConfig.gpio_cs = static_cast<gpio_num_t>(_cs);
        slotConfig.host_id = SPI2_HOST;

        sdmmc_host_t host = SDSPI_HOST_DEFAULT();

        ret = esp_vfs_fat_sdspi_mount(_mountPoint.c_str(), &host, &slotConfig, &mountConfig, &_sdCard);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to mount filesystem: %s", esp_err_to_name(ret));
            spi_bus_free(SPI2_HOST);
            return ret;
        }

        ESP_LOGI(TAG, "SD card mounted successfully at %s", _mountPoint.c_str());
        return ESP_OK;
    }

    esp_err_t unmount()
    {
        esp_err_t err = ESP_OK;

        if (_sdCard)
        {
            err = esp_vfs_fat_sdcard_unmount(_mountPoint.c_str(), _sdCard);
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to unmount SD card: %s", esp_err_to_name(err));
            }
            else
            {
                ESP_LOGI(TAG, "SD card unmounted successfully.");
            }

            _sdCard = nullptr;
        }
        else
        {
            ESP_LOGW(TAG, "No SD card to unmount.");
        }

        DisplayEXT7::getInstance()->setOutput(HW_EX_SD_CS, true);

        return err;
    }

    std::vector<std::string> listDirectory(const std::string &path) const
    {
        std::vector<std::string> files;
        DIR *dir = opendir((_mountPoint + path).c_str());
        if (dir)
        {
            struct dirent *entry;
            while ((entry = readdir(dir)) != nullptr)
            {
                files.emplace_back(entry->d_name);
            }
            closedir(dir);
        }
        else
        {
            ESP_LOGE(TAG, "Failed to open directory: %s", path.c_str());
        }
        return files;
    }

    std::string readFile(const std::string &path) const
    {
        std::string content;
        FILE *file = fopen((_mountPoint + path).c_str(), "r");
        if (file)
        {
            char buffer[1024];
            while (fgets(buffer, sizeof(buffer), file))
            {
                content += buffer;
            }
            fclose(file);
        }
        else
        {
            ESP_LOGE(TAG, "Failed to open file for reading: %s", path.c_str());
        }
        return content;
    }

    bool writeFile(const std::string &path, const std::string &data) const
    {
        FILE *file = fopen((_mountPoint + path).c_str(), "w");
        if (file)
        {
            fwrite(data.c_str(), 1, data.size(), file);
            fclose(file);
            return true;
        }
        else
        {
            ESP_LOGE(TAG, "Failed to open file for writing: %s", path.c_str());
            return false;
        }
    }

    bool deleteFile(const std::string &path) const
    {
        std::string fullPath = _mountPoint + path;
        if (remove(fullPath.c_str()) == 0)
        {
            ESP_LOGI(TAG, "File deleted: %s", fullPath.c_str());
            return true;
        }
        else
        {
            ESP_LOGE(TAG, "Failed to delete file: %s", fullPath.c_str());
            return false;
        }
    }
};
