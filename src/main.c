#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ds18b20_sensor.h"
#include "esp_log.h"

static const char *TAG = "MAIN";
#define WATER_TEMP_GPIO 4

void app_main(void)
{

    if (ds18b20_sensor_init(WATER_TEMP_GPIO) != ESP_OK) {
        ESP_LOGE(TAG, "Sensorinit misslyckades");
    }

    while (1) {
        float temps[2];
        if (ds18b20_sensor_read_all(temps, 2) == ESP_OK) {
            ESP_LOGI(TAG, "Sensor = Lufttemperatur (adress 8F0B2576714BFC28): %.2f C", temps[0]);
            ESP_LOGI(TAG, "Sensor = Vattentemperatur (adress 780B25764C430E28): %.2f C", temps[1]);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}