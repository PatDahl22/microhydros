#include "mqtt_manager.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include <stdio.h>

static const char *TAG = "MQTT";

// MQTT-brokern körs på Pattis dator i det lokala nätverket.
#define MQTT_BROKER_URI "mqtt://192.168.0.30:1883"

// Topic som används för MicroHydros mätdata.
#define MQTT_TOPIC "microhydros/measurements"

static esp_mqtt_client_handle_t mqtt_client = NULL;
static bool mqtt_connected = false;

/**
 * Hanterar MQTT-events från ESP-IDF.
 */
static void mqtt_event_handler(void *handler_args,
                               esp_event_base_t base,
                               int32_t event_id,
                               void *event_data)
{
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Connected to MQTT broker");
        mqtt_connected = true;
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "Disconnected from MQTT broker");
        mqtt_connected = false;
        break;

    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "Message published");
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT error");
        break;

    default:
        break;
    }
}

void mqtt_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);

    esp_mqtt_client_register_event(
        mqtt_client,
        ESP_EVENT_ANY_ID,
        mqtt_event_handler,
        NULL);

    esp_mqtt_client_start(mqtt_client);

    ESP_LOGI(TAG, "MQTT client started");
}

void mqtt_publish_measurements(
    float inside_temp,
    float inside_humidity,
    float outside_temp,
    float water_temp)
{
    if (!mqtt_connected)
    {
        ESP_LOGW(TAG, "Cannot publish - MQTT not connected");
        return;
    }

    char message[256];

    snprintf(
        message,
        sizeof(message),
        "{\"inside_temp\":%.2f,"
        "\"inside_humidity\":%.2f,"
        "\"outside_temp\":%.2f,"
        "\"water_temp\":%.2f,"
        "\"status\":\"ok\"}",
        inside_temp,
        inside_humidity,
        outside_temp,
        water_temp);

    int msg_id = esp_mqtt_client_publish(
        mqtt_client,
        MQTT_TOPIC,
        message,
        0,
        1,
        0);

    if (msg_id >= 0)
    {
        ESP_LOGI(TAG, "Published measurements: %s", message);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to publish measurements");
    }
}