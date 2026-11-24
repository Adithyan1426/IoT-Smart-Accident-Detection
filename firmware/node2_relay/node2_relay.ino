#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <string.h>
#include "../common/AccidentPacket.h"

uint8_t broadcastAddress[] = {255,255,255,255,255,255};

void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (len != sizeof(AccidentPacket)) return;
  AccidentPacket p;
  memcpy(&p, data, sizeof(p));
  p.hopCount = 2;
  p.node2 = 1;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t*)&p, sizeof(p));
  Serial.printf("Node 2 received seq=%lu; forward=%s\n",
    (unsigned long)p.sequence, result == ESP_OK ? "queued" : "failed");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
  if (esp_now_init() != ESP_OK) { Serial.println("ESP-NOW init failed"); while(true) delay(1000); }
  esp_now_register_recv_cb(onReceive);
  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, broadcastAddress, 6);
  peer.channel = ESPNOW_CHANNEL; peer.encrypt = false; peer.ifidx = WIFI_IF_STA;
  if (esp_now_add_peer(&peer) != ESP_OK) Serial.println("Peer add failed");
  Serial.println("Node 2 relay ready");
}
void loop() { delay(1000); }
