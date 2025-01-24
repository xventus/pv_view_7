//
// vim: ts=4 et
// Copyright (c) 2025 Petr Vanek, petr@fotoventus.cz
//
/// @file   hardware.h
/// @author Petr Vanek

#pragma once

#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/i2c.h"

// ESP32-S3-Touch-LCD-7
// https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-7

/*
Pin Connection
==============================================================================
| ESP32-S3-WROOM-x | LCD     | USB    | SD       | UART    | CAN    | Sensor |
|------------------|---------|--------|----------|---------|--------|--------|
| GPIO0            | G3      |        |          |         |        |        |
| GPIO1            | R3      |        |          |         |        |        |
| GPIO2            | R4      |        |          |         |        |        |
| GPIO3            | VSYNC   |        |          |         |        |        |
| GPIO4            | TP_IRQ  |        |          |         |        |        |
| GPIO5            | DE      |        |          |         |        |        |
| GPIO6            |         |        |          |         |        | AD     |
| GPIO7            | PCLK    |        |          |         |        |        |
| GPIO8            | TP_SDA  |        |          |         |        |        |
| GPIO9            | TP_SCL  |        |          |         |        |        |
| GPIO10           | B7      |        |          |         |        |        |
| GPIO11           |         |        | MOSI     |         |        |        |
| GPIO12           |         |        | SCK      |         |        |        |
| GPIO13           |         |        | MISO     |         |        |        |
| GPIO14           | B3      |        |          |         |        |        |
| GPIO15           |         |        |          | RS485_TX|        |        |
| GPIO16           |         |        |          | RS485_RX|        |        |
| GPIO17           | B6      |        |          |         |        |        |
| GPIO18           | B5      |        |          |         |        |        |
| GPIO19           |         | USB_DN |          |         | CANRX  |        |
| GPIO20           |         | USB_DP |          |         | CANTX  |        |
| GPIO21           | G7      |        |          |         |        |        |
| GPIO38           | B4      |        |          |         |        |        |
| GPIO39           | G2      |        |          |         |        |        |
| GPIO40           | R7      |        |          |         |        |        |
| GPIO41           | R6      |        |          |         |        |        |
| GPIO42           | R5      |        |          |         |        |        |
| GPIO43           |         |        |          | UART_TXD|        |        |
| GPIO44           |         |        |          | UART_RXD|        |        |
| GPIO45           | G4      |        |          |         |        |        |
| GPIO46           | HSYNC   |        |          |         |        |        |
| GPIO47           | G6      |        |          |         |        |        |
| GPIO48           | G5      |        |          |         |        |        |
| CH422G           |         |        |          |         |        |        |
| EXIO1            | TP_RST  |        |          |         |        |        |  
| EXIO2            | DISP    |        |          |         |        |        |
| EXIO3            | LCD_RST |        |          |         |        |        |
| EXIO4            |         |        | SD_CS    |         |        |        |
| EXIO5            |         | USB_SEL|          |         |        |        |
==============================================================================
*/

/*
Description CH422

EXIO1      TP_RST      LCD    -> LOW -> HIGH
EXIO2      DISP        LCD    -> HIGH
EXIO3      LCD_RST     LCD    -> HIGH
EXIO4      SD_CS       SD     -> 
EXIO5      USB_SEL     USB    -> LOW

*/


#define HW_EX_TP_RST    1
#define HW_EX_DISP      2
#define HW_EX_LCD_RST   3
#define HW_EX_SD_CS     4

// SD CARD

#define HW_SD_MOSI GPIO_NUM_11
#define HW_SD_MISO GPIO_NUM_13
#define HW_SD_CLK  GPIO_NUM_12
#define HW_SD_CS  GPIO_NUM_NC

// DISPLAY

#define HW_SDA  GPIO_NUM_8 
#define HW_SCL  GPIO_NUM_9

#define HW_I2C_NUM           i2c_port_t::I2C_NUM_0 
#define HW_I2C_TIMEOUT_MS    1000

#define HW_LCD_HSYNC           46
#define HW_LCD_VSYNC           3
#define HW_LCD_DE              5
#define HW_LCD_PCLK            7

// BLUE
#define HW_LCD_DATA_0           14
#define HW_LCD_DATA_1           38 
#define HW_LCD_DATA_2           18 
#define HW_LCD_DATA_3           17 
#define HW_LCD_DATA_4           10 

// GREEN
#define HW_LCD_DATA_5           39 
#define HW_LCD_DATA_6           0  
#define HW_LCD_DATA_7           45 
#define HW_LCD_DATA_8           48 
#define HW_LCD_DATA_9           47 
#define HW_LCD_DATA_10          21 

// RED
#define HW_LCD_DATA_11          1  
#define HW_LCD_DATA_12          2  
#define HW_LCD_DATA_13          42 
#define HW_LCD_DATA_14          41 
#define HW_LCD_DATA_15          40 


#define HW_LCD_H_RES                   800 
#define HW_LCD_V_RES                   480


/*
PCLK=(HSYNC_time_row)×(VSYNC_number_of_rows)×(refresh) 
HSYNC_time_row = 800  + back_porch + front_porch + hsync_pulse_width = aprox. 1056 pixels
VSYNC_number_of_rows = 480 + back_porch + front_porch + vsync_pulse_width = aprox. 525 rows

60Hz:  PCLK=1056×525×60=33,264,000Hz
30Hz: aprox. 
 */
#define HW_LCD_PIXEL_CLOCK          (18 * 1000 * 1000)

#define HW_LCD_CTP_IRQ             GPIO_NUM_4
#define HW_LCD_CTP_IRQ_SEL          1ULL<<HW_LCD_CTP_IRQ


#define LCD_PIN_NUM_DISP_EN         -1