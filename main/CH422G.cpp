#include "CH422G.h"
#include "esp_log.h"
#include "hardware.h"



CH422G::CH422G(i2c_port_t i2cPort)
    : _i2cPort(i2cPort), _directionReg(DEFAULT_DIR), _outputReg(DEFAULT_OUT) {
    _mutex = xSemaphoreCreateMutex();
}


CH422G::~CH422G() {
    if (_mutex) {
        vSemaphoreDelete(_mutex);
    }
}

esp_err_t CH422G::init(uint8_t defaultOutput, uint8_t config) {
    ESP_LOGI(TAG, "Initializing CH422G...");
    
    esp_err_t ret = configureChip(config);
    if (ret != ESP_OK) {
        return ret;
    }

    _outputReg = defaultOutput;
    return writeRegister(REG_OUT, _outputReg);
}

esp_err_t CH422G::configureChip(uint8_t config) {
    ESP_LOGI(TAG, "Configuring CH422G with value 0x%02X...", config);
    return writeRegister(REG_SYS, config);
}

esp_err_t CH422G::writeRegister(uint8_t reg, uint8_t value) {
    
    ESP_LOGW(TAG, "writeRegister [0x%x] -> [0x%x]", reg, value);
    xSemaphoreTake(_mutex, portMAX_DELAY);
    esp_err_t ret = i2c_master_write_to_device(_i2cPort, reg, &value, 1, HW_I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
    xSemaphoreGive(_mutex);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write register 0x%02X: %s", reg, esp_err_to_name(ret));
    }
    return ret;
}


esp_err_t CH422G::setOutput(uint8_t data) {
    _outputReg = data;
    return writeRegister(REG_OUT, _outputReg);
}

esp_err_t CH422G::setOutput(uint8_t pin, bool state) {
    if (pin >= 8) {
        ESP_LOGE(TAG, "Invalid pin: %d", pin);
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t mask = (1 << pin);
    if (state) {
        _outputReg |= mask; // Set bit
    } else {
        _outputReg &= ~mask; // Clear bit
    }

    return writeRegister(REG_OUT, _outputReg);
}

