#ifndef SHAREDTYPES_H
#define SHAREDTYPES_H

#include <stdint.h>

enum class PacketType : uint8_t { KeyDataHalf, KeyDataFull, Ping, Pong, Count };

enum class DeviceID : uint8_t { KeyboardLeft, KeyboardRight, Dongle, Count };

struct KeyData {
  bool topLeftPressed = false;
  bool topRightPressed = false;
  bool bottomLeftPressed = false;
  bool bottomRightPressed = false;
};

struct PingPacket {
  uint16_t sequence;     // Increment for each ping
  uint32_t timestamp_ms; // millis() when sent
  // Optional:
  DeviceID identifier; // Session ID (useful if multiple devices ping)
};

struct PongPacket {
  uint16_t sequence;     // Echo back the sequence
  uint32_t timestamp_ms; // Echo back original timestamp
  // Optional:
  uint32_t recv_time_ms; // When pong-er received the ping
  uint32_t send_time_ms; // When pong-er sent the pong
};

#endif