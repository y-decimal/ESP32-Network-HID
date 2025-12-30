#ifndef EVENTTYPES_H
#define EVENTTYPES_H
#include <stdint.h>
#include <system/SystemConfig.h>

enum class EventType : uint8_t { Key, BitMap, ConfigUpdate, COUNT };

struct KeyEvent {
  uint16_t keyIndex;
  bool state;
};

struct BitMapEvent {
  uint8_t bitMapSize;
  uint8_t bitMap[BITMAPSIZE];
};

struct Event {
  EventType type;
  union {
    KeyEvent key;
    BitMapEvent bitMap;
  };
};

#endif