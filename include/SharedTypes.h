#ifndef SHAREDTYPES_H
#define SHAREDTYPES_H

#include <stdint.h>

enum class PacketType : uint8_t { KeyDataHalf, KeyDataFull, Ping, Pong, Count };

enum class DeviceID : uint8_t { KeyboardLeft, KeyboardRight, Dongle, Count };

#endif