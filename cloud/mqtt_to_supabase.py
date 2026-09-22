import json
import os

import paho.mqtt.client as mqtt
from dotenv import load_dotenv
from supabase import create_client


load_dotenv(os.path.join(os.path.dirname(__file__), ".env"))

SUPABASE_URL = os.getenv("SUPABASE_URL")
SUPABASE_SECRET_KEY = os.getenv("SUPABASE_SECRET_KEY")

if not SUPABASE_URL or not SUPABASE_SECRET_KEY:
    raise RuntimeError("Supabase credentials are missing from cloud/.env")

supabase = create_client(SUPABASE_URL, SUPABASE_SECRET_KEY)

MQTT_BROKER = "127.0.0.1"
MQTT_PORT = 1883
MQTT_TOPIC = "microhydros/measurements"


def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT broker")
        client.subscribe(MQTT_TOPIC)
        print(f"Subscribed to {MQTT_TOPIC}")
    else:
        print(f"MQTT connection failed: {rc}")


def on_message(client, userdata, message):
    try:
        payload = json.loads(message.payload.decode("utf-8"))

        measurement = {
            "inside_temp": payload.get("inside_temp"),
            "inside_humidity": payload.get("inside_humidity"),
            "outside_temp": payload.get("outside_temp"),
            "water_temp": payload.get("water_temp"),
            "status": payload.get("status", "unknown"),
        }

        supabase.table("measurements").insert(measurement).execute()

        print(f"Saved measurement: {measurement}")

    except Exception as error:
        print(f"Failed to save measurement: {error}")


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

print("Connecting to MQTT broker...")
client.connect(MQTT_BROKER, MQTT_PORT, 60)
client.loop_forever()
