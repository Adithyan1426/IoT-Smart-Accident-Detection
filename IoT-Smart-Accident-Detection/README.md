# IoT-Based Smart Accident Detection and Alert System

Three ESP32 nodes acquire motion/GPS data, relay telemetry over ESP-NOW, and deliver it to a Raspberry Pi Flask server for SQLite storage and browser monitoring.

> Educational prototype only. The included X-axis threshold is a demonstration baseline, not a validated vehicle-safety algorithm. Do not rely on it as a sole emergency system.

## Architecture
Node 1 (ESP32 + MPU6050 + NEO-6M + buzzer)
→ ESP-NOW → Node 2 (ESP32 relay)
→ ESP-NOW → Node 3 (ESP32 gateway)
→ Wi-Fi / HTTP JSON → Raspberry Pi (Flask + SQLite)
→ Web dashboard

## Structure
- `firmware/common/` shared ESP-NOW packet definition
- `firmware/node1_sensor/` sensing, GPS and buzzer
- `firmware/node2_relay/` packet forwarding
- `firmware/node3_gateway/` HTTP gateway to Raspberry Pi
- `raspberry_pi/` Flask API, SQLite and dashboard
- `docs/` architecture, wiring and test plan
- `tests/` API smoke test

## Hardware / wiring (Node 1)
| Module | Connection |
|---|---|
| MPU6050 SDA / SCL | GPIO 21 / GPIO 22 |
| MPU6050 VCC / GND | 3V3 / GND (check breakout requirements) |
| NEO-6M TX / RX | GPIO 16 / GPIO 17 |
| Buzzer driver input | GPIO 25 |

Use a transistor/MOSFET driver if the buzzer current exceeds GPIO limits. GPS UART is 9600 baud; MPU6050 address defaults to `0x68`.

## Arduino dependencies
Install ESP32 board support, Adafruit MPU6050, Adafruit Unified Sensor, and TinyGPSPlus. All three boards must use compatible ESP-NOW channel settings.

## Configure Node 3
Edit `WIFI_SSID`, `WIFI_PASSWORD`, and `PI_URL` in `firmware/node3_gateway/node3_gateway.ino`. Replace the sample Pi address with the actual Raspberry Pi IP.

**Radio note:** ESP-NOW and station-mode Wi-Fi share the ESP32 radio. Node 3 follows the Wi-Fi access point's channel; configure Nodes 1 and 2 to use the same 2.4 GHz channel. The starter sketches use channel 6.

## Raspberry Pi setup
```bash
cd raspberry_pi
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
python app.py
```
Open `http://<PI_IP>:5000/`.

## API
- `POST /api/telemetry` — ingest JSON
- `GET /api/data` — latest record
- `GET /api/history?limit=100` — recent records
- `GET /health` — health check
- `GET /` — dashboard

The API starter has no authentication or TLS. Keep it on a trusted network and do not commit credentials, personal GPS logs, or `.env` files. Record only tests actually performed in `docs/testing.md`.
