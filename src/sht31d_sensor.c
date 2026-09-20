#include "sht31d_sensor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "SHT31D";
static i2c_master_dev_handle_t sht31d_device = NULL;
static i2c_master_bus_handle_t sht31d_bus_handle = NULL;

#define SHT31D_ADDR      0x44
#define CMD_MEASURE_MSB  0x24   
#define CMD_MEASURE_LSB  0x00
#define I2C_FREQ_HZ      100000

// verifiera att mätdata inte är korrupt
static uint8_t sht31d_crc8(const uint8_t *data, int len)
{
    uint8_t crc = 0xFF;
    for (int i = 0; i < len; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++) {
            crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : (crc << 1);
        }
    }
    return crc;
}

esp_err_t sht31d_sensor_init(int sda_gpio, int scl_gpio)
{
    
    //skapar en I2C-buss för sht31d
    
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = -1,
        .scl_io_num = scl_gpio,
        .sda_io_num = sda_gpio,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    esp_err_t err = i2c_new_master_bus(&bus_config, &sht31d_bus_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Kunde inte skapa I2C-bus: %s", esp_err_to_name(err));
        return err;
    }

    i2c_device_config_t device_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = SHT31D_ADDR,
        .scl_speed_hz = I2C_FREQ_HZ,
    };

    err = i2c_master_bus_add_device(sht31d_bus_handle, &device_config, &sht31d_device);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Kunde inte lagga till SHT31-D pa bussen: %s", esp_err_to_name(err));
        return err;
    }

   
    // Verifiera att sensorn faktiskt svarar på bussen

 
    err = i2c_master_probe(sht31d_bus_handle, SHT31D_ADDR, 1000);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "SHT31-D svarar inte pa 0x%02X: %s", SHT31D_ADDR, esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "SHT31-D redo pa I2C-adress 0x%02X (SDA=%d, SCL=%d)", SHT31D_ADDR, sda_gpio, scl_gpio);
    return ESP_OK;
}

    esp_err_t sht31d_sensor_read(float *temperature, float *humidity)
    {
        if (sht31d_device == NULL) {
            ESP_LOGE(TAG, "Sensorn ar inte initierad");
            return ESP_ERR_INVALID_STATE;
        }

        uint8_t cmd[2] = { CMD_MEASURE_MSB, CMD_MEASURE_LSB };
        esp_err_t err = i2c_master_transmit(sht31d_device, cmd, sizeof(cmd), 1000);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Kunde inte starta matning: %s", esp_err_to_name(err));
            return err;
        }

        vTaskDelay(pdMS_TO_TICKS(20)); // Sensorn behover ~15-20 ms for high repeatability

        uint8_t data[6];
        err = i2c_master_receive(sht31d_device, data, sizeof(data), 1000);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Kunde inte lasa matdata: %s", esp_err_to_name(err));
            return err;
        }

        //  CRC for bade temp och fuktighet
        
        if (sht31d_crc8(&data[0], 2) != data[2] || sht31d_crc8(&data[3], 2) != data[5]) {
            ESP_LOGE(TAG, "CRC-fel i matdata fran SHT31-D, sensorn kan vara felaktig");
            return ESP_ERR_INVALID_CRC;
        }

        uint16_t raw_temp = (data[0] << 8) | data[1];
        uint16_t raw_hum  = (data[3] << 8) | data[4];

        *temperature = -45.0f + 175.0f * ((float)raw_temp / 65535.0f);
        *humidity    = 100.0f * ((float)raw_hum / 65535.0f);

        ESP_LOGI(TAG, "Temp: %.2f C, Fuktighet: %.2f %%", *temperature, *humidity);

        return ESP_OK;
    }