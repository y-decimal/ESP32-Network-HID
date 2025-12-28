#ifndef EVENTTYPES_H
#define EVENTTYPES_H

#include <stdint.h>

enum class EventType : uint8_t { Key, ConfigUpdate, COUNT };

struct KeyEvent {
  uint16_t keyIndex;
  bool state;
};

struct Event {
  EventType type;
  union {
    KeyEvent key;
  };
};

#endif