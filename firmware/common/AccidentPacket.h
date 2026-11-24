#pragma once
#include <stdint.h>
#define ESPNOW_CHANNEL 6

typedef struct __attribute__((packed)) {
  int32_t senderID;
  int32_t hopCount;
  uint32_t sequence;
  uint8_t accident;
  uint8_t reserved[3];
  float ax, ay, az;
  float lat, lng, speed, altitude;
  int32_t satellites;
  int32_t node1, node2;
} AccidentPacket;

static_assert(sizeof(AccidentPacket) == 56, "Packet must be 56 bytes");
