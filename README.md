# MicroHydros

IoT- och embeddedprototyp för övervakning av en hydroponisk odlingsmiljö.

## Grupp 4
- Mahdi Ahmadi
- Ossian Petermann
- Pattaravarat Dahl

## Syfte
Syftet med projektet är att utveckla och utvärdera en mindre, stabil IoT- och embeddedprototyp för övervakning av en hydroponisk odlingsmiljö. Systemet ska samla in relevanta miljömätningar återkommande, behandla mätdata i en embedded-enhet och möjliggöra överföring av data till ett externt system.

Projektet ska även ge underlag för att bedöma valda sensorers lämplighet, kommunikationslösningens funktion samt möjliga begränsningar och framtida vidareutveckling.

## Projektmål
Prototypen ska kunna:
- mäta lufttemperatur inne i odlingsmiljön,
- mäta relativ luftfuktighet inne i odlingsmiljön,
- mäta lufttemperatur utanför odlingsmiljön,
- mäta temperatur i vatten/näringslösning,
- genomföra mätningarna återkommande med ett definierat mätintervall,
- behandla och strukturera mätdata i embedded-enheten,
- överföra mätdata till ett externt system,
- hantera vissa felaktiga eller orimliga sensorvärden,
- dokumenteras och testas så att lösningens funktion, arkitektur och begränsningar kan utvärderas.

## Teknisk lösning
Gruppen har valt följande huvudkomponenter och kommunikationslösning:
- ESP32-S3 som embedded-plattform,
- SHT31-D för lufttemperatur och relativ luftfuktighet inne,
- DS18B20 för lufttemperatur utanför odlingsmiljön,
- DS18B20 i vattentätt utförande för temperatur i vatten/näringslösning,
- Freenove LCD1602 med I²C för lokal visning av mätdata,
- Wi-Fi för nätverksanslutning,
- MQTT för överföring av mätdata till en extern MQTT-broker.

De två DS18B20-sensorerna kan dela samma 1-Wire-buss och använder ett 4,7 kΩ pull-up-motstånd. SHT31-D och LCD1602 kommunicerar via I²C.

## Systemarkitektur

```text
SHT31-D ── I²C ───────┐
LCD1602 ── I²C ───────┤
                      ▼
                  ESP32-S3
                      ▲
DS18B20 ute ── 1-Wire ┤
DS18B20 vatten ───────┘
                      │
                    Wi-Fi
                      │
                      ▼
                 MQTT Broker
                      │
                      ▼
              Externt system
```

ESP32-S3 ansvarar för sensoravläsning, behandling och validering av mätdata samt kommunikation till det externa systemet.

## Prioritering och vidareutveckling
Projektets kärnfunktioner prioriteras först: samtliga fyra mätpunkter, återkommande mätningar, embedded-behandling, felhantering och extern kommunikation.

LCD1602 används som lokal display men får inte prioriteras framför kärnfunktionerna.

Om tid finns efter att kärnfunktionerna fungerar stabilt är målet att även lagra historisk mätdata i en molntjänst. Molnlagring betraktas som vidareutveckling och är inte ett krav för projektets grundläggande fungerande prototyp.

## Repositorystruktur
- `docs/` – projektplan, krav, sensorval, arkitektur och testning
- `firmware/` – kod för ESP32-S3
- `hardware/` – kopplingar, komponenter och hårdvarudokumentation

## Arbetssätt
Vi arbetar iterativt med GitHub Issues som backlog. Arbetsuppgifter prioriteras och utvecklas stegvis. Funktioner utvecklas i separata branches och granskas via Pull Requests där det är lämpligt.

## Status
Projektstart: 31 augusti 2026  
Slutleverans: 25 september 2026
