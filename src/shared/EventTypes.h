#ifndef EVENTTYPES_H
#define EVENTTYPES_H
#include <cstdlib>
#include <stdint.h>

enum class EventType : uint8_t { Key, BitMap, ConfigUpdate, COUNT };

struct KeyEvent{
  uint16_t keyIndex;
  bool state;
  uint8_t sourceMac[6];
};

struct BitMapEvent {
  uint8_t bitMapSize;
  uint8_t *bitMapData;
};

struct Event {
  EventType type;

  void (*cleanup)(Event *);

  union {
    KeyEvent key;
    BitMapEvent bitMap;
  };
};

inline void cleanupKeyEvent(Event *event) { return; }
inline void cleanupBitmapEvent(Event *event) { free(event->bitMap.bitMapData); }

#endif