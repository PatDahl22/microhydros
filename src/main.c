#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "LCD.h"



void app_main(void)
{
    LCD_init();

    float temperature = 15.5;
    char lcd_temp[32];

    float humidity = 62;
    char lcd_humidity[32];

    snprintf(lcd_temp, sizeof(lcd_temp), "Temp: %.1f C", temperature);

    snprintf(lcd_humidity, sizeof(lcd_humidity), "Humidity: %.1f", humidity);

    LCD(0, 0, lcd_temp);
    
    LCD(0, 1, lcd_humidity);
    

}


    //printf("MicroHydros boot OK (ESP-IDF)\n");

    //while (1) {
    //    vTaskDelay(pdMS_TO_TICKS(1000));
    //}