# Integrationstest – MicroHydros

## Syfte

Syftet med integrationstestet är att verifiera att MicroHydros-systemets
huvudkomponenter fungerar tillsammans: sensorer, ESP32-S3, Wi-Fi och MQTT.

## Testmiljö

- ESP32-S3 DevKitC-1
- SHT31-D för inomhustemperatur och luftfuktighet
- Två DS18B20 för vatten-/näringslösningstemperatur och utomhustemperatur
- Wi-Fi
- Mosquitto MQTT-broker på lokal dator
- MQTT-topic: `microhydros/measurements`

## Testresultat

| Test | Förväntat resultat | Faktiskt resultat | Status |
| --- | --- | --- | --- |
| Alla fyra mätvärden | Systemet ska läsa inomhustemperatur, luftfuktighet, utomhustemperatur och vattentemperatur | Alla fyra mätvärden lästes korrekt | Godkänt |
| Återkommande mätning | Nya mätningar ska göras återkommande | Mätvärden lästes kontinuerligt med cirka 3 sekunders intervall | Godkänt |
| DS18B20-identitet | Systemet ska kunna skilja vatten- och luftsensorn åt | Sensor 0 identifierades som vatten och sensor 1 som luft | Godkänt |
| SHT31-D | Temperatur och luftfuktighet ska kunna läsas | Båda värdena lästes och gav rimliga resultat | Godkänt |
| MQTT-publicering | Mätdata ska skickas från ESP32-S3 till ett externt system | JSON-meddelanden publicerades till `microhydros/measurements` | Godkänt |
| MQTT-mottagning | Publicerad data ska kunna tas emot utanför den inbyggda enheten | Meddelanden verifierades med Mosquitto subscriber på datorn | Godkänt |
| DS18B20-fel | Systemet ska upptäcka problem när en temperatursensor kopplas bort | Läs-/CRC-fel registrerades när sensorn kopplades bort | Godkänt |
| SHT31-D-fel | Systemet ska upptäcka när SHT31-D inte kan läsas | Felet upptäcktes och rapporterades i loggen | Godkänt |
| Ofullständig mätning | Ogiltig/ofullständig data ska inte publiceras som en normal komplett mätning | MQTT-publicering hoppades över när mätningen var ofullständig | Godkänt |

## Exempel på MQTT-data

```json
{
  "inside_temp": 21.68,
  "inside_humidity": 46.94,
  "outside_temp": 21.25,
  "water_temp": 13.06,
  "status": "ok"
}

## Slutsats

### Integrationstestet visar att MicroHydros kärnflöde fungerar från
sensoravläsning på ESP32-S3 till överföring av mätdata till ett externt
system via Wi-Fi och MQTT.

### Systemet kan läsa de fyra obligatoriska mätvärdena återkommande och
publicera dem som strukturerad JSON-data. Tester med bortkopplade sensorer
visade även att fel kan upptäckas och att ofullständiga mätningar inte
publiceras som normala mätvärden.

### Under integrationen upptäcktes även problem kring delning av I2C-bussen
mellan LCD och SHT31-D. Detta dokumenteras som en integrationsbegränsning
och hanteras separat från MQTT-funktionaliteten.
