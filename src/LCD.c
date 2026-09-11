#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_rom_sys.h"

#include "LCD.h"




static const char *TAG = "LCD";

static i2c_master_bus_handle_t bus_handle;
static i2c_master_dev_handle_t lcd_handle;

/* PCF8574 pins */
#define LCD_RS  0x01
#define LCD_EN  0x04
#define LCD_BL  0x08

static void lcd_write_byte(uint8_t data)
{
    i2c_master_transmit(lcd_handle, &data, 1, -1);
}

static void lcd_pulse_enable(uint8_t data)
{
    lcd_write_byte(data | LCD_EN);
    esp_rom_delay_us(1);
    lcd_write_byte(data & ~LCD_EN);
    esp_rom_delay_us(50);
}

static void lcd_write4(uint8_t nibble, uint8_t rs)
{
    uint8_t data = (nibble & 0xF0) | LCD_BL;

    if (rs)
        data |= LCD_RS;

    lcd_pulse_enable(data);
}

static void lcd_send(uint8_t value, uint8_t rs)
{
    lcd_write4(value & 0xF0, rs);
    lcd_write4((value << 4) & 0xF0, rs);
}

static void lcd_command(uint8_t cmd)
{
    lcd_send(cmd, 0);
    vTaskDelay(pdMS_TO_TICKS(2));
}

static void lcd_data(uint8_t data)
{
    lcd_send(data, 1);
}

static void lcd_init(void)
{
    vTaskDelay(pdMS_TO_TICKS(50));

    // Initialize LCD in 4-bit mode
    lcd_write4(0x30, 0);
    vTaskDelay(pdMS_TO_TICKS(5));

    lcd_write4(0x30, 0);
    esp_rom_delay_us(150);

    lcd_write4(0x30, 0);
    vTaskDelay(pdMS_TO_TICKS(1));

    lcd_write4(0x20, 0);

    // 4-bit, 2 lines, 5x8 font
    lcd_command(0x28);

    // Display off
    lcd_command(0x08);

    // Clear display
    lcd_command(0x01);

    // Entry mode
    lcd_command(0x06);

    // Display on, cursor off, blink off
    lcd_command(0x0C);
}

static void lcd_set_cursor(uint8_t col, uint8_t row)
{
    uint8_t address;

    if (row == 0)
        address = 0x00 + col;
    else
        address = 0x40 + col;

    lcd_command(0x80 | address);
}

static void lcd_print(const char *str)
{
    while (*str)
        lcd_data((uint8_t)*str++);
}

void LCD_init(void){
        // Create I2C bus
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = I2C_SDA_GPIO,
        .scl_io_num = I2C_SCL_GPIO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    ESP_ERROR_CHECK(
        i2c_new_master_bus(&bus_config, &bus_handle)
    );

    // Add LCD to bus
    i2c_device_config_t lcd_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = LCD_ADDR,
        .scl_speed_hz = I2C_FREQ_HZ,
    };

    ESP_ERROR_CHECK(
        i2c_master_bus_add_device(
            bus_handle,
            &lcd_config,
            &lcd_handle
        )
    );


    ESP_LOGI(TAG, "Initializing LCD");

    lcd_init();

}

void LCD(int col, int row, const char *line)
{
    
    lcd_set_cursor(col, row);

    for (int i = 0; i < 16; i++){
        lcd_data(' ');
    }

    lcd_set_cursor(col, row);
    
    lcd_print(line);

}