# System architecture

1. Node 1 samples MPU6050 acceleration, parses NEO-6M GPS data, applies the demonstration threshold, drives a buzzer, and broadcasts telemetry.
2. Node 2 receives and rebroadcasts the packet with hop metadata.
3. Node 3 receives telemetry and POSTs JSON to the Raspberry Pi Flask API.
4. Flask validates and stores records in SQLite; the dashboard polls the read endpoints.

**Radio constraint:** ESP-NOW and station-mode Wi-Fi share the ESP32 radio. Node 3 follows the access point's channel. Nodes 1 and 2 must use the same 2.4 GHz channel, or channel coordination must be implemented.
