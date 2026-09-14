#pragma once 
#include "esp_err.h"
#include "driver/i2c.h"

esp_err_t sht31d_sensor_init(i2c_port_t port, int sda_gpio, int scl_gpio);
esp_err_t sht31d_sensor_read(i2c_port_t port, float *temperature, float *humidity);