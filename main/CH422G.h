#pragma once

#include <cstdint>
#include "driver/i2c.h"
#include "esp_err.h"

/**
 * @class CH422G
 * @brief Simplified class for managing the CH422G I/O expander via I2C.
 * NOTE: ONLY for OUTPUT now
 */
class CH422G
{

    /**
     * @brief Writes a value to a CH422G register.
     * @param reg Register address.
     * @param value Value to write.
     * @return ESP_OK on success, or an error code.
     */
    esp_err_t writeRegister(uint8_t reg, uint8_t value);

 public:
    /**
     * @brief Constructor for CH422G.
     * @param i2cPort I2C port number.
     * @param i2cAddress I2C address of the device.
     */
    CH422G(i2c_port_t i2cPort);

    /// @brief dtor
    virtual ~CH422G();
    

    /**
     * System Parameter Register (0x24) Settings:
     * -----------------------------------------
     * This register controls the main functionality of the CH422G chip,
     * including sleep mode, output mode, dynamic scanning, and IO direction.
     * 
     * Bit Definitions:
     * Bit | Function     | Description
     * ----|--------------|-------------------------------------------------
     *  7  | SLEEP        | 1 = Sleep mode, 0 = Active mode
     *  6  | (Reserved)   | Must be 0
     *  5  | OD_EN        | 1 = OC3-OC0 in open-drain mode, 0 = Push-pull mode
     *  4  | (Reserved)   | Must be 0
     *  3  | A_SCAN       | 1 = Enable dynamic display scanning, 0 = I/O mode
     *  2  | (Reserved)   | Must be 0
     *  1  | IO_OE        | 1 = IO7-IO0 as outputs, 0 = IO7-IO0 as inputs
     *  0  | IO_OE        | 1 = IO7-IO0 as outputs, 0 = IO7-IO0 as inputs
     * 
     * Common Configurations:
     * Value (Hex) | SLEEP | OD_EN | A_SCAN | IO_OE | Description
     * ------------|-------|-------|--------|-------|---------------------------------------------
     * 0x01        |   0   |   0   |   0    |  01   | Default: IO7-IO0 as outputs, push-pull, active mode, I/O expander mode.
     * 0x00        |   0   |   0   |   0    |  00   | IO7-IO0 as inputs, push-pull, active mode, I/O expander mode.
     * 0x03        |   0   |   0   |   1    |  01   | IO7-IO0 as outputs, push-pull, dynamic display scanning enabled.
     * 0x21        |   1   |   0   |   0    |  01   | IO7-IO0 as outputs, push-pull, sleep mode enabled.
     * 0x05        |   0   |   1   |   0    |  01   | IO7-IO0 as outputs, OC3-OC0 in open-drain mode, active mode.
     * 0x04        |   0   |   1   |   0    |  00   | IO7-IO0 as inputs, OC3-OC0 in open-drain mode, active mode.
     * 0x23        |   1   |   0   |   1    |  01   | IO7-IO0 as outputs, push-pull, sleep mode and dynamic scanning enabled.
     * 0x07        |   0   |   1   |   1    |  01   | IO7-IO0 as outputs, OC3-OC0 in open-drain mode, dynamic scanning enabled.
     * 
     * Note:
     * - For most I/O expander applications, the default value (0x01) is sufficient.
     * - To enable dynamic display scanning, set A_SCAN (bit 3) to 1.
     * - Use open-drain mode (OD_EN = 1) for OC3-OC0 if external pull-up resistors are required.
     */

    /**
     * @brief Initializes the CH422G device.
     * @param defaultOutput  Default output after init.
     * @param config  Chip configuration.
     * @return ESP_OK on success, or an error code.
     */
    esp_err_t init(uint8_t defaultOutput = 0x00, uint8_t config = 0x01);

    /**
     * @brief Sets the state of a specific output pin.
     * @param pin Pin number (0-7).
     * @param state True for HIGH, false for LOW.
     * @return ESP_OK on success, or an error code.
     */
    esp_err_t setOutput(uint8_t pin, bool state);


    /**
     * @brief Sets dat.
     * @param pin Pin number (0-7).
     * @param data write new value.
     * @return ESP_OK on success, or an error code.
     */
    esp_err_t setOutput(uint8_t data);
   
    /**
     * @brief Configures the system parameters of CH422G.
     * @param config Value to set in the system parameter register (0x24).
     * @return ESP_OK on success, or an error code.
     */
    esp_err_t configureChip(uint8_t config);


    /// @brief gets shared mutex for i2c
    /// @return 
    SemaphoreHandle_t getMutex() const { return _mutex; }

private:
    static constexpr const char *TAG = "CH422G";

    i2c_port_t _i2cPort;   ///< I2C port number.
    uint8_t _directionReg; ///< Direction register value (default 0xFF for all inputs).
    uint8_t _outputReg;    ///< Output register value 
    SemaphoreHandle_t _mutex; ///< Mutex for thread-safe access.
    

    static constexpr uint8_t REG_IN = 0x26;        ///< Input register address.
    static constexpr uint8_t REG_OUT = 0x38;       ///< Output register address.
    static constexpr uint8_t REG_SYS = 0x24;       ///< System parameter register address.
    static constexpr uint8_t DEFAULT_DIR = 0xFF;   ///< Default direction register value.
    static constexpr uint8_t DEFAULT_OUT = 0x00;   ///< Default output register value.
};
