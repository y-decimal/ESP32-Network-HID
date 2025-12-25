#ifndef SHAREDTYPES_H
#define SHAREDTYPES_H

#include <stdint.h>

enum class PacketType : uint8_t { KeyDataHalf, KeyDataFull, Count };

enum class DeviceID : uint8_t { KeyboardLeft, KeyboardRight, Dongle, Count };

#endif