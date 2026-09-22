# Cloud Storage – MicroHydros

## Overview

As an optional extension to the MicroHydros system, historical measurement data is stored in a cloud database.

The purpose of the cloud storage is to demonstrate that measurement data from the ESP32 can be collected over time and later retrieved for analysis.

The cloud solution is not required for the core functionality of the system. The ESP32 continues to perform sensor measurements and publish MQTT messages independently of the cloud storage.

## Architecture

The measurement data follows this flow:

ESP32-S3 → Wi-Fi → MQTT → Mosquitto → Python bridge → Supabase

The ESP32-S3 reads the connected sensors and publishes the measurements as JSON to the MQTT topic:

`microhydros/measurements`

A Mosquitto MQTT broker runs on a computer on the local network.

A small Python application, `cloud/mqtt_to_supabase.py`, subscribes to the MQTT topic. When a new MQTT message is received, the JSON payload is parsed and stored as a new row in the Supabase database.

This keeps the cloud integration separate from the embedded firmware. The ESP32 only needs to communicate using MQTT and does not need to connect directly to the cloud database.

## Database

Supabase was selected as the cloud database service.

The database contains a `measurements` table with the following fields:

- `id` – unique identifier for each measurement.
- `created_at` – timestamp generated when the measurement is stored.
- `inside_temp` – inside air temperature.
- `inside_humidity` – inside relative humidity.
- `outside_temp` – outside air temperature.
- `water_temp` – water/nutrient temperature.
- `status` – measurement status, for example `ok` or `sensor_error`.

Each received MQTT measurement is stored as a separate row. This creates a historical record of measurements over time.

## MQTT to Supabase bridge

The Python bridge uses the following libraries:

- `paho-mqtt` for subscribing to MQTT messages.
- `supabase` for communication with the Supabase database.
- `python-dotenv` for loading configuration from environment variables.

The required Python dependencies are listed in:

`cloud/requirements.txt`

The bridge can be started from the project root with:

```powershell
python cloud/mqtt_to_supabase.py
```

When running, the application connects to the local MQTT broker, subscribes to `microhydros/measurements`, and stores received measurements in Supabase.

## Configuration and secrets

Supabase credentials are stored locally in:

`cloud/.env`

This file is excluded from Git using `.gitignore` and must not be committed to the repository.

An example configuration is provided in:

`cloud/.env.example`

The example file contains placeholders only and can safely be stored in the repository.

## Historical data

Each measurement stored in Supabase includes a timestamp. This makes it possible to view measurements collected at different times and use them for later analysis.

During testing, measurements published by the ESP32 through MQTT were successfully received by the Python bridge and stored as new rows in the Supabase `measurements` table.

The stored data could then be viewed in the Supabase Table Editor.

## Error handling

The cloud storage supports the measurement status produced by the MicroHydros system.

Normal measurements are stored with:

`status = ok`

If a sensor measurement is unavailable or invalid, the MQTT message can contain `null` for that value and use:

`status = sensor_error`

Because the database measurement columns allow null values, partial measurements can still be stored without replacing missing sensor data with misleading numeric values.

## Choice of cloud solution

For this project, Supabase was chosen instead of a larger IoT cloud platform such as AWS IoT or Azure IoT.

AWS IoT and Azure IoT provide more advanced functionality for device management, authentication, scaling and cloud integration. However, introducing these services would also add configuration and complexity that were not necessary for the scope of this project.

The main purpose of the optional cloud functionality was to demonstrate persistent historical storage of sensor measurements.

The combination of MQTT, a small Python bridge and Supabase provides a simple solution that:

- works with the existing MQTT implementation,
- keeps cloud-specific code outside the ESP32 firmware,
- provides persistent database storage,
- allows historical measurements to be viewed easily,
- and can be explained and demonstrated within the project scope.

For a larger production system, a dedicated IoT cloud platform could be evaluated as a future development.

## Limitations

The current Python bridge must be running for MQTT measurements to be transferred to Supabase.

The Mosquitto broker is also running locally, so the current implementation is primarily a prototype rather than a production cloud architecture.

If the bridge or network connection is unavailable, measurements published during that period are not automatically recovered and stored later.

Despite these limitations, the solution demonstrates the complete flow from an embedded sensor system to persistent historical cloud storage.
