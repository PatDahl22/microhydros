# Technical Documentation – MicroHydros

## Wi-Fi and MQTT communication

The MicroHydros system uses Wi-Fi to send measurement data from the ESP32-S3 to an external system.

Wi-Fi is initialized during system startup after NVS has been initialized. The ESP32 connects to the configured local wireless network before MQTT communication is used.

MQTT is used to publish the collected sensor measurements to a Mosquitto broker running on a computer in the local network.

### MQTT configuration

The system publishes measurement data to the following topic:

`microhydros/measurements`

Measurements are sent as JSON. A normal measurement can for example look like:

```json
{
  "inside_temp": 22.53,
  "inside_humidity": 39.97,
  "outside_temp": 19.88,
  "water_temp": 22.25,
  "status": "ok"
}
```

The MQTT client keeps track of its connection state. If the client is not connected to the broker, the system logs a warning instead of attempting to publish measurement data. Sensor measurements continue even when MQTT is temporarily unavailable.

When the MQTT connection becomes available, subsequent measurements can be published normally.

## Error handling and validation

Basic error handling and validation were implemented to prevent invalid sensor readings from being treated as normal measurement data.

Temperature values are validated against a defined reasonable range, and humidity is validated between 0–100 %. If a sensor read fails or returns an unreasonable value, the affected measurement is marked as invalid internally using `NAN`.

When publishing measurements over MQTT:

- Valid measurements are published with `"status":"ok"`.
- Invalid or unavailable measurements are represented as `null`.
- If one or more measurements are invalid, the message is published with `"status":"sensor_error"`.

Example during a sensor error:

```json
{
  "inside_temp": 22.49,
  "inside_humidity": 40.02,
  "outside_temp": null,
  "water_temp": null,
  "status": "sensor_error"
}
```

The error handling was tested by disconnecting and reconnecting the DS18B20 sensors and the SHT31-D sensor while the system was running. The system continued operating with the remaining available measurements and automatically returned to `"status":"ok"` when valid sensor readings were available again.
