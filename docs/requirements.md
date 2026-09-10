# Kravspecifikation – MicroHydros

## Funktionella krav

| ID | Krav | Prioritet |

|--- |--- |--- |
| FR-01 | Systemet ska mäta lufttemperatur inne i odlingsmiljön. | Must |
| FR-02 | Systemet ska mäta relativ luftfuktighet inne i odlingsmiljön. | Must |
| FR-03 | Systemet ska mäta lufttemperatur utanför odlingsmiljön. | Must |
| FR-04 | Systemet ska mäta temperaturen i vatten/näringslösning. | Must |
| FR-05 | Systemet ska utföra mätningarna återkommande med ett definierat intervall. | Must |
| FR-06 | Systemet ska läsa in och behandla mätdata i embedded-enheten. | Must |
| FR-07 | Mätdata ska struktureras så att mätpunkt, värde och status kan identifieras. | Must |
| FR-08 | Systemet ska kunna skicka mätdata till ett externt system. | Must |
| FR-09 | Systemet ska kunna upptäcka och hantera vissa felaktiga eller orimliga sensorvärden. | Must |

## Begränsningar

### CON-01 – Sensorbegränsning

DHT11 och DHT22 får inte användas i lösningen.

## Fastställda teknikval

## Motivering av teknik- och sensorval

### ESP32-S3

ESP32-S3 valdes som embedded-plattform eftersom den har inbyggt Wi-Fi och stöd för de gränssnitt som behövs i projektet. Plattformen kan kommunicera med SHT31-D via I²C och med DS18B20-sensorerna via 1-Wire.

Inbyggt Wi-Fi gör det möjligt att överföra mätdata via MQTT utan ytterligare nätverkshårdvara. Plattformen ger även möjlighet att senare utöka prototypen med fler sensorer eller funktioner.

Projektet använder ESP32-S3 DevKitC-1 N16R8 och utvecklingen sker med PlatformIO och ESP-IDF.

### SHT31-D

SHT31-D valdes för mätning av lufttemperatur och relativ luftfuktighet inne i odlingsmiljön. En sensor kan därmed täcka två av projektets fyra obligatoriska mätpunkter.

Sensorn använder ett digitalt I²C-gränssnitt, vilket gör den lämplig för integration med ESP32-S3. Valet gjordes även eftersom projektet kräver ett alternativ till DHT11 och DHT22, som enligt kundkravet inte får användas.

Eftersom sensorn ska användas i en miljö med förhöjd luftfuktighet behöver placering och eventuell påverkan från kondens beaktas vid testerna.

### DS18B20

DS18B20 valdes för mätning av lufttemperaturen utanför odlingsmiljön samt temperaturen i vatten/näringslösningen.

Sensorn använder det digitala 1-Wire-gränssnittet. Varje DS18B20 har en unik adress, vilket gör det möjligt att ansluta båda temperatursensorerna på samma 1-Wire-buss och ändå identifiera respektive mätpunkt.

För mätningen i vatten/näringslösningen används en kapslad vattentät variant av DS18B20. Detta gör den praktisk för prototypen eftersom själva sensorn kan placeras i vätskan.

En begränsning är att en vattentät kapsling inte automatiskt innebär att sensorn och kabeln är specificerade för långvarig exponering för alla typer av näringslösningar. Detta behöver därför dokumenteras som en begränsning i prototypen.

### Wi-Fi och MQTT

Wi-Fi används för nätverksanslutning och MQTT används för överföring av mätdata från ESP32-S3 till ett externt system.

MQTT är lämpligt för återkommande överföring av sensorvärden och använder en publish/subscribe-modell. ESP32-S3 kan publicera mätdata till en MQTT-broker utan att behöva känna till vilka system som senare använder informationen.

Det gör lösningen möjlig att vidareutveckla med exempelvis datalagring eller visualisering utan att den grundläggande kommunikationen från embedded-enheten behöver byggas om.

### LCD1602

LCD1602 med I²C används som en extra funktion för lokal visning av aktuell mätinformation och systemstatus.

Displayen är inte nödvändig för att uppfylla projektets grundläggande krav och prioriteras därför efter sensormätning, databehandling, felhantering och MQTT-kommunikation.

### Molnlagring

Lagring av historisk mätdata i en molntjänst betraktas som möjlig vidareutveckling. Funktionen implementeras endast om projektets kärnfunktioner först fungerar stabilt och tillräcklig tid finns kvar.

## Spårbarhet

| Krav | Implementation | Verifiering |

|---|---|---|
| FR-01 | SHT31-D – Issue #2 | Sensor- och integrationstest |
| FR-02 | SHT31-D – Issue #2 | Sensor- och integrationstest |
| FR-03 | DS18B20 – Issue #3 | Sensor- och integrationstest |
| FR-04 | DS18B20 – Issue #3 | Sensor- och integrationstest |
| FR-05 | Mätcykel – Issue #4 | Test av återkommande mätningar |
| FR-06 | Databehandling – Issue #4 | Kontroll av behandlade mätvärden |
| FR-07 | Datastruktur – Issue #4 | Kontroll av strukturerad mätdata |
| FR-08 | Wi-Fi och MQTT – Issue #5 | Kommunikationstest mot externt system |
| FR-09 | Validering och felhantering – Issue #6 | Test med felaktiga/orimliga sensorvärden |
| CON-01 | DHT11/DHT22 används inte | Kontroll av implementation och hårdvara |
