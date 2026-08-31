# MicroHydros

IoT- och embeddedprototyp för övervakning av en hydroponisk odlingsmiljö.

## Projektmål
Prototypen ska kunna:
- mäta lufttemperatur inne i odlingsmiljön,
- mäta relativ luftfuktighet inne i odlingsmiljön,
- mäta lufttemperatur utanför odlingsmiljön,
- mäta temperatur i vatten/näringslösning,
- genomföra mätningar återkommande,
- behandla mätdata i ett embedded-system,
- kommunicera mätdata till ett externt system,
- hantera vissa felaktiga eller orimliga situationer.

## Preliminär teknisk lösning
- ESP32-S3
- SHT31-D för temperatur och relativ luftfuktighet inne
- DS18B20 för lufttemperatur ute
- Vattentät DS18B20 för vattentemperatur
- Wi-Fi och preliminärt MQTT för extern kommunikation

> Sensor- och kommunikationsval är preliminära tills gruppen har jämfört alternativ och dokumenterat beslutet.

## Repositorystruktur
- `docs/` – projektplan, krav, sensorval, arkitektur och testning
- `firmware/` – kod för ESP32-S3
- `hardware/` – kopplingar, komponenter och hårdvarudokumentation

## Arbetssätt
Vi arbetar iterativt med GitHub Issues som backlog. Funktioner utvecklas i separata branches och granskas via Pull Requests där det är lämpligt.

## Status
Projektstart: 31 augusti 2026  
Slutleverans: 25 september 2026
