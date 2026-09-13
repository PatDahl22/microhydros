#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "LCD.h"


void app_main(void)
{
    // Give serial monitor time to connect.
    vTaskDelay(pdMS_TO_TICKS(3000));

    printf("\nMicroHydros LCD test\n");


#if CONFIG_IDF_TARGET_ESP32C6

    printf("Board: ESP32-C6\n");

#elif CONFIG_IDF_TARGET_ESP32S3

    printf("Board: ESP32-S3\n");

#endif


    // Initialize LCD.
    esp_err_t err = LCD_init();

    if (err != ESP_OK)
    {
        printf(
            "LCD initialization failed: %s\n",
            esp_err_to_name(err)
        );

        return;
    }


    // Show test text.
    LCD_clear();

    LCD_print_at(
        0,
        0,
        "MICROHYDROS"
    );

    LCD_print_at(
        0,
        1,
        "LCD WORKS :)"
    );


    printf("LCD text sent\n");


    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}