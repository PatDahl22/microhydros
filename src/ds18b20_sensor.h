#pragma once
#include "esp_err.h"

esp_err_t ds18b20_sensor_init(int gpio_num);
esp_err_t ds18b20_sensor_read_all(float *temps_out, int max_count);