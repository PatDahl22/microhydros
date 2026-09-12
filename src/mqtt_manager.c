#include "mqtt_manager.h"

#include "esp_log.h"
#include "mqtt_client.h"

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
        NULL
    );

    esp_mqtt_client_start(mqtt_client);

    ESP_LOGI(TAG, "MQTT client started");
}


void mqtt_publish_test(void)
{
    if (!mqtt_connected)
    {
        ESP_LOGW(TAG, "Cannot publish - MQTT not connected");
        return;
    }

    const char *message =
        "{\"status\":\"hello from ESP32-S3\"}";

    esp_mqtt_client_publish(
        mqtt_client,
        MQTT_TOPIC,
        message,
        0,
        1,
        0
    );

    ESP_LOGI(TAG, "Published: %s", message);
}