# IoT Weather Station: ESP32-C6 & Spring Boot

## 📖 Project Overview
A telemetry system based on the ESP32-C6 microcontroller. The system collects environmental data using Deep Sleep energy optimization and transmits it to a central server.

## 🏗️ Architecture
- **Edge Layer:** FireBeetle 2 ESP32-C6 (C++, ESP-IDF). Measurement every 10 minutes.
- **Message Broker:** Eclipse Mosquitto (MQTT).
- **Backend Layer:** Java 21, Spring Boot 3.x (Spring Integration MQTT).
- **Persistence Layer:** PostgreSQL with TimescaleDB plugin.
- **Visualization:** Grafana.
- **Infrastructure:** Docker Compose (orchestration).

## 🔌 Hardware
- DFRobot FireBeetle 2 ESP32-C6
- BME280 (Temperature, Humidity, Pressure) - I2C bus
- GY-302 / BH1750 (Light intensity) - I2C bus
- DIY Radiation shield (made from white plant pot saucers or 3D printed)

## 🗺️ Development Roadmap

### Phase 1: Infrastructure (Docker Compose)
- [x] Define services: Mosquitto, PostgreSQL, Grafana.
- [x] Configure internal Docker networks for secure communication.
- [x] Bind persistent volumes to retain database state and Grafana dashboards.

### Phase 2: Edge Layer (ESP32-C6)
- [x] Initialize ESP-IDF project structure.
- [x] Write I2C drivers for BME280 and GY-302 sensors.
- [x] Implement WiFi connection and MQTT publishing logic.
- [x] Configure Deep Sleep mode (10-minute cycles).

### Phase 3: Backend Layer (Java 21 / Spring Boot)
- [x] Design PostgreSQL relational schema. 
- [x] Configure Spring Integration MQTT inbound adapter.
- [ ] Implement payload parsing and database insertion logic.

### Phase 4: Visualization (Grafana)
- [ ] Connect Grafana directly to the PostgreSQL instance.
- [ ] Design real-time and historical dashboards.

### Phase 5: Phisical Part
- [ ] Solder elements.
- [ ] Do/print radiation shield.
- [ ] Montage elements inside of the radiation shield.

### 🔮 Phase 6: Future Enhancements (TODO)
- [ ] Implement OTA (Over-The-Air) firmware updates.
- [ ] Add battery voltage monitoring via ADC.
- [ ] Set up alerting in Grafana (e.g., frost or low battery warnings).

## 🚀 Getting Started

### 1. Project Cloning
- Copy repository:
```bash
git clone https://github.com/wrzepka/esp32-weather-station.git
cd esp32-weather-station
```
- Fill up .env file

### 2. Infrastructure (Docker)
Start the environment (Broker, DB, Grafana, Backend):
```bash
cd infrastructure
docker compose --env-file "../.env" up -d --build
```

### 3. Edge (ESP-IDF)
Compile and flash ESP32:
```bash
cd ../firmware
pio run -t upload -t monitor
```
