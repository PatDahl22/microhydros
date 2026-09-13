#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

/**
 * @brief Startar MQTT-klienten och ansluter till MicroHydros-brokern.
 */
void mqtt_start(void);

/**
 * @brief Publicerar ett testmeddelande till MicroHydros MQTT-topic.
 */
void mqtt_publish_test(void);

#endif