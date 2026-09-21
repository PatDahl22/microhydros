#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ds18b20_sensor.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_err.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "sht31d_sensor.h"
#include "LCD.h"

#define WATER_TEMP_GPIO 4
#define SHT31D_SDA_GPIO 15
#define SHT31D_SCL_GPIO 16

static const char *TAG = "MAIN";

void app_main(void)
{
    vTaskDelay(pdMS_TO_TICKS(1000));

    // test för sht31d sesnor isf om kopplingen är fel
    if (sht31d_sensor_init(SHT31D_SDA_GPIO, SHT31D_SCL_GPIO) != ESP_OK)
    {
        ESP_LOGE(TAG, "SHT31-D sensor lyckades inte koppla");
    }

    // Initialise the LCD display

    LCD_init();

    vTaskDelay(pdMS_TO_TICKS(3000));

    printf("MicroHydros boot OK\n");
    fflush(stdout);

    // ------------------------------------------------
    // Initialize NVS
    // ESP-IDF Wi-Fi behöver NVS innan Wi-Fi startas.
    // ------------------------------------------------
    esp_err_t ret = nvs_flash_init();

    // Om NVS-partitionen är gammal/full: radera och initiera på nytt.
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    printf("NVS initialized OK\n");
    fflush(stdout);

    // ------------------------------------------------
    // Start Wi-Fi och MQTT
    // ------------------------------------------------
    wifi_init_sta();
    mqtt_start();

    // ------------------------------------------------
    // Initiera DS18B20-sensorerna
    // ------------------------------------------------
    if (ds18b20_sensor_init(WATER_TEMP_GPIO) != ESP_OK)
    {
        ESP_LOGE(TAG, "Koppling misslyckades");
    }

    // Sensor Vattentempratur =(adress 8F0B2576714BFC28)
    // Sensor Lufttemperatur =(adress 780B25764C430E28)

    while (1)
    {

        float temps[2];
        float humidity = 0.0f;
        float temp_in = 0.0f;

        char lcd_temp[32];
        char lcd_hum[32];

        if (ds18b20_sensor_read_all(temps, 2) == ESP_OK)
        {
            ESP_LOGI(TAG, "Sensor 0 = Vattentempratur: %.2f C", temps[0]);
            ESP_LOGI(TAG, "Sensor 1 = Lufttemperatur: %.2f C", temps[1]);
        }

        if (sht31d_sensor_read(&temp_in, &humidity) == ESP_OK)
        {
            ESP_LOGI(TAG, "SHT31-D: Innetemp: %.2f C, Fuktighet: %.2f %%", temp_in, humidity);
        }

        snprintf(lcd_temp, sizeof(lcd_temp), "T:%.1f IN:%.1f", temps[1], temp_in);
        snprintf(lcd_hum, sizeof(lcd_hum), "H:%.1f W:%.1f", humidity, temps[0]);

        LCD_clear();
        LCD_print_at(0, 0, lcd_temp);
        LCD_print_at(0, 1, lcd_hum);

        mqtt_publish_measurements(
            temp_in,
            humidity,
            temps[1],
            temps[0]);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}