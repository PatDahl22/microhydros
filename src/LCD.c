#include "LCD.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "freertos/semphr.h"
static SemaphoreHandle_t lcd_mutex = NULL;

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "rom/ets_sys.h"


// ----------------------------------------------------
// PCF8574 -> LCD1602 mapping
//
// Confirmed mapping from MAP 01:
//
// P0 = RS
// P1 = RW
// P2 = Enable
// P3 = Backlight
// P4 = D4
// P5 = D5
// P6 = D6
// P7 = D7
// ----------------------------------------------------

#define LCD_RS 0x01
#define LCD_RW 0x02
#define LCD_EN 0x04
#define LCD_BL 0x08

#define LCD_D4 0x10
#define LCD_D5 0x20
#define LCD_D6 0x40
#define LCD_D7 0x80


static i2c_master_dev_handle_t lcd_device = NULL;
static i2c_master_bus_handle_t lcd_bus_handle = NULL;

// ----------------------------------------------------
// Write one byte to PCF8574
// ----------------------------------------------------

static bool pcf8574_write(uint8_t value)
{
    esp_err_t err = i2c_master_transmit(
        lcd_device,
        &value,
        1,
        1000
    );

    if (err != ESP_OK)
    {
        printf(
            "LCD I2C error: %s\n",
            esp_err_to_name(err)
        );

        return false;
    }

    return true;
}


// ----------------------------------------------------
// Convert LCD nibble to PCF8574 bits
// ----------------------------------------------------

static uint8_t lcd_map_nibble(uint8_t nibble)
{
    uint8_t value = LCD_BL;

    if (nibble & 0x01)
    {
        value |= LCD_D4;
    }

    if (nibble & 0x02)
    {
        value |= LCD_D5;
    }

    if (nibble & 0x04)
    {
        value |= LCD_D6;
    }

    if (nibble & 0x08)
    {
        value |= LCD_D7;
    }

    return value;
}


// ----------------------------------------------------
// Pulse Enable
// ----------------------------------------------------

static void lcd_pulse_enable(uint8_t value)
{
    pcf8574_write(value | LCD_EN);

    ets_delay_us(10);

    pcf8574_write(value & ~LCD_EN);

    ets_delay_us(100);
}


// ----------------------------------------------------
// Send one 4-bit nibble
// ----------------------------------------------------

static void lcd_send_nibble(
    uint8_t nibble,
    bool data_mode
)
{
    uint8_t value = lcd_map_nibble(nibble);

    if (data_mode)
    {
        value |= LCD_RS;
    }

    lcd_pulse_enable(value);
}


// ----------------------------------------------------
// Send complete byte
// ----------------------------------------------------

static void lcd_send_byte(
    uint8_t value,
    bool data_mode
)
{
    lcd_send_nibble(
        value >> 4,
        data_mode
    );

    lcd_send_nibble(
        value & 0x0F,
        data_mode
    );
}


// ----------------------------------------------------
// Send LCD command
// ----------------------------------------------------

static void lcd_command(uint8_t command)
{
    lcd_send_byte(
        command,
        false
    );

    if (command == 0x01 || command == 0x02)
    {
        vTaskDelay(pdMS_TO_TICKS(3));
    }
    else
    {
        ets_delay_us(100);
    }
}


// ----------------------------------------------------
// Send one character
// ----------------------------------------------------

static void lcd_write_char(char character)
{
    lcd_send_byte(
        (uint8_t)character,
        true
    );
}


// ----------------------------------------------------
// Initialize LCD
// ----------------------------------------------------

esp_err_t LCD_init(void)
{
    printf("LCD: starting initialization\n");

    // ---------------------------------------------
    // Create I2C bus
    // ---------------------------------------------

    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = -1,
        .scl_io_num = I2C_SCL_GPIO,
        .sda_io_num = I2C_SDA_GPIO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };


    esp_err_t err = i2c_new_master_bus(
        &bus_config,
        &lcd_bus_handle
    );

    if (err != ESP_OK)
    {
        printf(
            "LCD: I2C bus init failed: %s\n",
            esp_err_to_name(err)
        );

        return err;
    }


    // ---------------------------------------------
    // Add PCF8574 device
    // ---------------------------------------------

    i2c_device_config_t device_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = LCD_ADDR,
        .scl_speed_hz = I2C_FREQ_HZ,
    };

    err = i2c_master_bus_add_device(
        lcd_bus_handle,
        &device_config,
        &lcd_device
    );

    if (err != ESP_OK)
    {
        printf(
            "LCD: device init failed: %s\n",
            esp_err_to_name(err)
        );

        return err;
    }


    printf(
        "LCD: PCF8574 ready at 0x%02X\n",
        LCD_ADDR
    );

    lcd_mutex = xSemaphoreCreateMutex();

    // ---------------------------------------------
    // HD44780 initialization
    // ---------------------------------------------

    vTaskDelay(pdMS_TO_TICKS(100));

    // Backlight on
    pcf8574_write(LCD_BL);

    // Reset sequence
    lcd_send_nibble(0x03, false);
    vTaskDelay(pdMS_TO_TICKS(10));

    lcd_send_nibble(0x03, false);
    vTaskDelay(pdMS_TO_TICKS(5));

    lcd_send_nibble(0x03, false);
    vTaskDelay(pdMS_TO_TICKS(5));

    // Switch to 4-bit mode
    lcd_send_nibble(0x02, false);
    vTaskDelay(pdMS_TO_TICKS(5));

    // 4-bit mode, 2 lines, 5x8 font
    lcd_command(0x28);

    // Display off
    lcd_command(0x08);

    // Clear display
    lcd_command(0x01);

    // Cursor moves right
    lcd_command(0x06);

    // Display on, cursor off
    lcd_command(0x0C);

    printf("LCD: initialization complete\n");

    return ESP_OK;
}


// ----------------------------------------------------
// Clear display
// ----------------------------------------------------

void LCD_clear(void)
{
    if (lcd_mutex == NULL){
        return;
    }
    xSemaphoreTake(lcd_mutex, portMAX_DELAY);

    lcd_command(0x01);

    xSemaphoreGive(lcd_mutex);
}




// ----------------------------------------------------
// Set cursor position
//
// col = 0-15
// row = 0-1
// ----------------------------------------------------

void LCD_set_cursor(
    uint8_t col,
    uint8_t row
)
{
    if (col >= 16 || row >=2){
        return;
    }

    uint8_t address;

    if (row == 0)
    {
        address = 0x80 + col;
    }
    else
    {
        address = 0xC0 + col;
    }

    lcd_command(address);
}


// ----------------------------------------------------
// Print string at current cursor position
// ----------------------------------------------------

void LCD_print(const char *text)
{
    while (*text)
    {
        lcd_write_char(*text);
        text++;
    }
}


// ----------------------------------------------------
// Print string at selected position
// ----------------------------------------------------

void LCD_print_at(
    uint8_t col,
    uint8_t row,
    const char *text
)
{
    LCD_set_cursor(
        col,
        row
    );
    
    LCD_print(text);
}

i2c_master_bus_handle_t LCD_get_i2c_bus(void)
{
    return lcd_bus_handle;
}

void lcd_clear_small(void){
    LCD_set_cursor(0,0);
    for (int i = 0; i<16; i++){
        lcd_write_char(' ');
    }
    LCD_set_cursor(0,1);
    for (int i = 0; i < 16; i++){
        lcd_write_char(' ');
    }

    LCD_set_cursor(0,0);
}

void LCD_print_all(const char *line1, const char *line2){
    if (lcd_mutex == NULL){
        return;
    }

    xSemaphoreTake(lcd_mutex, portMAX_DELAY);

    lcd_clear_small();

    LCD_print_at(0,0,line1);
    LCD_print_at(0,1,line2);

    xSemaphoreGive(lcd_mutex);

}