#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"

esp_err_t sht31d_sensor_init(int sda_gpio, int scl_gpio);
esp_err_t sht31d_sensor_read(float *temperature, float *humidity);
