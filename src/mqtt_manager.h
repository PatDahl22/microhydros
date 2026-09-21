#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

/**
 * @brief Startar MQTT-klienten och ansluter till MicroHydros-brokern.
 */
void mqtt_start(void);

/**
 * @brief Publicerar aktuella mätvärden till MicroHydros MQTT-topic.
 */
void mqtt_publish_measurements(
    float inside_temp,
    float inside_humidity,
    float outside_temp,
    float water_temp);

#endif