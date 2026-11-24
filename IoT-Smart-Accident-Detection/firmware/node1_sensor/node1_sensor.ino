#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <TinyGPSPlus.h>
#include "../common/AccidentPacket.h"

#define BUZZER_PIN 25
#define GPS_RX 16
#define GPS_TX 17
const float AX_THRESHOLD_G = 1.0f; // Demonstration only; calibrate mounting/orientation.

Adafruit_MPU6050 mpu;
TinyGPSPlus gps;
HardwareSerial GPSSerial(2);
uint32_t sequenceNumber = 0;
uint8_t broadcastAddress[] = {255,255,255,255,255,255};

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  Wire.begin(21, 22);
  GPSSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  if (!mpu.begin(0x68, &Wire)) {
    Serial.println("MPU6050 not detected");
    while (true) delay(1000);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    while (true) delay(1000);
  }
  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, broadcastAddress, 6);
  peer.channel = ESPNOW_CHANNEL;
  peer.encrypt = false;
  peer.ifidx = WIFI_IF_STA;
  if (esp_now_add_peer(&peer) != ESP_OK) Serial.println("Broadcast peer add failed");
  Serial.println("Node 1 ready");
}

void loop() {
  while (GPSSerial.available()) gps.encode(GPSSerial.read());
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  float ax = a.acceleration.x / 9.80665f;
  float ay = a.acceleration.y / 9.80665f;
  float az = a.acceleration.z / 9.80665f;
  bool accident = ax < AX_THRESHOLD_G;
  digitalWrite(BUZZER_PIN, accident ? HIGH : LOW);

  AccidentPacket p = {};
  p.senderID = 1; p.hopCount = 1; p.sequence = ++sequenceNumber;
  p.accident = accident ? 1 : 0;
  p.ax = ax; p.ay = ay; p.az = az;
  p.lat = gps.location.isValid() ? gps.location.lat() : 0;
  p.lng = gps.location.isValid() ? gps.location.lng() : 0;
  p.speed = gps.speed.isValid() ? gps.speed.kmph() : 0;
  p.altitude = gps.altitude.isValid() ? gps.altitude.meters() : 0;
  p.satellites = gps.satellites.isValid() ? gps.satellites.value() : 0;
  p.node1 = 1;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t*)&p, sizeof(p));
  Serial.printf("seq=%lu ax=%.2f ay=%.2f az=%.2f accident=%u send=%s gps=%s\n",
    (unsigned long)p.sequence, ax, ay, az, p.accident,
    result == ESP_OK ? "queued" : "failed", gps.location.isValid() ? "valid" : "no-fix");
  delay(500);
}
