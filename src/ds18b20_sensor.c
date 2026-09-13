#include "ds18b20_sensor.h"
#include "onewire_bus.h"
#include "ds18b20.h"
#include "esp_log.h"

#define MAX_DS18B20 2

static const char *TAG = "DS18B20";
static onewire_bus_handle_t bus = NULL;
static ds18b20_device_handle_t sensors[MAX_DS18B20];
static int sensor_count = 0;

esp_err_t ds18b20_sensor_init(int gpio_num)
{
    onewire_bus_config_t bus_config = {
        .bus_gpio_num = gpio_num,
    };
    onewire_bus_rmt_config_t rmt_config = {
        .max_rx_bytes = 10,
    };

    esp_err_t err = onewire_new_bus_rmt(&bus_config, &rmt_config, &bus);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Kunde inte skapa 1-Wire-bus: %s", esp_err_to_name(err));
        return err;
    }

    onewire_device_iter_handle_t iter = NULL;
    onewire_device_t next_device;
    esp_err_t search_result;

    ESP_ERROR_CHECK(onewire_new_device_iter(bus, &iter));
    ESP_LOGI(TAG, "Söker efter DS18B20-enheter...");

    do {
        search_result = onewire_device_iter_get_next(iter, &next_device);
        if (search_result == ESP_OK) {
            ds18b20_config_t ds_cfg = {};
            if (ds18b20_new_device_from_enumeration(&next_device, &ds_cfg, &sensors[sensor_count]) == ESP_OK) {
                ESP_LOGI(TAG, "Hittade DS18B20[%d], adress: %016llX", sensor_count, next_device.address);
                sensor_count++;
                if (sensor_count >= MAX_DS18B20) break;
            }
        }
    } while (search_result != ESP_ERR_NOT_FOUND);

    ESP_ERROR_CHECK(onewire_del_device_iter(iter));
    ESP_LOGI(TAG, "Sökning klar, %d DS18B20 hittade", sensor_count);

    return sensor_count > 0 ? ESP_OK : ESP_ERR_NOT_FOUND;
}

esp_err_t ds18b20_sensor_read_all(float *temps_out, int max_count)
{
    if (sensor_count == 0) return ESP_ERR_INVALID_STATE;

    esp_err_t err = ds18b20_trigger_temperature_conversion_for_all(bus);
    if (err != ESP_OK) return err;

    int count = sensor_count < max_count ? sensor_count : max_count;
    for (int i = 0; i < count; i++) {
        ESP_ERROR_CHECK(ds18b20_get_temperature(sensors[i], &temps_out[i]));
    }
    return ESP_OK;
}