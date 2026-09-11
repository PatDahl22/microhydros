#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "LCD.h"



void app_main(void)
{
    LCD_init();

    float temp_out = 15.5;
    char lcd_temp[32];

    float temp_w = 15.3;

    float temp_in = 15.2;
    float humidity = 62.5;
    char lcd_humidity[32];

    snprintf(lcd_temp, sizeof(lcd_temp), "T:%.1fC In %.1f", temp_out, temp_in);

    snprintf(lcd_humidity, sizeof(lcd_humidity), "WT:%.1f H: %f.1",  temp_w, humidity);

    LCD(0, 0, lcd_temp);
    
    LCD(0, 1, lcd_humidity);
    

}


    //printf("MicroHydros boot OK (ESP-IDF)\n");

    //while (1) {
    //    vTaskDelay(pdMS_TO_TICKS(1000));
    //}