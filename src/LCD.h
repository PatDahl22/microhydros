#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include "esp_err.h"

// ----------------------------------------------------
// Board-specific I2C pins
// ----------------------------------------------------

#if CONFIG_IDF_TARGET_ESP32C6

#define I2C_SDA_GPIO 6
#define I2C_SCL_GPIO 7

#elif CONFIG_IDF_TARGET_ESP32S3

#define I2C_SDA_GPIO 8
#define I2C_SCL_GPIO 9

#else

#error "Unsupported ESP32 target"

#endif


// ----------------------------------------------------
// Common LCD configuration
// ----------------------------------------------------

#define I2C_FREQ_HZ 100000
#define LCD_ADDR     0x27


// ----------------------------------------------------
// Public functions
// ----------------------------------------------------

esp_err_t LCD_init(void);

void LCD_clear(void);

void LCD_set_cursor(
    uint8_t col,
    uint8_t row
);

void LCD_print(
    const char *text
);

void LCD_print_at(
    uint8_t col,
    uint8_t row,
    const char *text
);

#endif