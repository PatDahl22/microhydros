#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "esp_err.h"

#include "wifi_manager.h"
#include "mqtt_manager.h"

void app_main(void)
{
    // Vänta lite så Serial Monitor hinner ansluta
    vTaskDelay(pdMS_TO_TICKS(3000));

    printf("MicroHydros boot OK\n");
    fflush(stdout);

    // -------------------------------------------------
    // Initialize NVS
    // ESP-IDF Wi-Fi behöver NVS innan Wi-Fi startas.
    // -------------------------------------------------
    esp_err_t ret = nvs_flash_init();

    // Om NVS-partitionen är gammal/full:
    // radera den och initiera på nytt.
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    printf("NVS initialized OK\n");
    fflush(stdout);

    // -------------------------------------------------
    // Start Wi-Fi
    // -------------------------------------------------
    wifi_init_sta();

    mqtt_start();

    while (1)
    {
        mqtt_publish_test();

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}