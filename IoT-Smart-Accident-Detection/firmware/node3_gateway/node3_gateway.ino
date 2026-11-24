#include <WiFi.h>
#include <esp_now.h>
#include <HTTPClient.h>
#include <string.h>
#include "../common/AccidentPacket.h"

// Set these before flashing. Use a trusted 2.4 GHz network.
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* PI_URL = "http://192.168.1.50:5000/api/telemetry";

volatile bool packetReady = false;
AccidentPacket latestPacket;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (len != sizeof(AccidentPacket)) return;
  portENTER_CRITICAL(&mux);
  memcpy(&latestPacket, data, sizeof(latestPacket));
  packetReady = true;
  portEXIT_CRITICAL(&mux);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Wi-Fi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(500); Serial.print(".");
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("Pi gateway connected; AP channel=%d\n", WiFi.channel());
  } else Serial.println("Wi-Fi not connected");

  if (esp_now_init() != ESP_OK) { Serial.println("ESP-NOW init failed"); while(true) delay(1000); }
  esp_now_register_recv_cb(onReceive);
  Serial.println("Node 3 ready");
}

void loop() {
  if (!packetReady) { delay(10); return; }
  AccidentPacket p;
  portENTER_CRITICAL(&mux);
  p = latestPacket; packetReady = false;
  portEXIT_CRITICAL(&mux);

  if (WiFi.status() != WL_CONNECTED) { Serial.println("No Wi-Fi; packet dropped"); return; }
  String body = "{";
  body += "\"sender_id\":" + String(p.senderID) + ",";
  body += "\"hop_count\":" + String(p.hopCount) + ",";
  body += "\"sequence\":" + String(p.sequence) + ",";
  body += "\"accident\":" + String(p.accident) + ",";
  body += "\"ax\":" + String(p.ax, 4) + ",";
  body += "\"ay\":" + String(p.ay, 4) + ",";
  body += "\"az\":" + String(p.az, 4) + ",";
  body += "\"lat\":" + String(p.lat, 6) + ",";
  body += "\"lng\":" + String(p.lng, 6) + ",";
  body += "\"speed\":" + String(p.speed, 2) + ",";
  body += "\"altitude\":" + String(p.altitude, 2) + ",";
  body += "\"satellites\":" + String(p.satellites) + ",";
  body += "\"gps_valid\":" + String((p.lat != 0 || p.lng != 0) ? "true" : "false") + "}";
  HTTPClient http;
  http.begin(PI_URL);
  http.addHeader("Content-Type", "application/json");
  int code = http.POST(body);
  Serial.printf("POST HTTP status: %d\n", code);
  http.end();
}
