#ifndef EVENTTYPES_H
#define EVENTTYPES_H
#include <cstdlib>
#include <stdint.h>

enum class EventType : uint8_t
{
  Key,
  BitMap,
  ConfigUpdate,
  COUNT
};

struct RawKeyEvent
{
  uint16_t keyIndex;
  bool state;
};

struct IdentifiableKeyEvent
{
  RawKeyEvent raw;
  const uint8_t *sourceMac;
};

struct RawBitmapEvent
{
  uint8_t bitMapSize;
  uint8_t *bitMapData;
};

struct IdentifiableBitMapEvent
{
  RawBitmapEvent raw;
  const uint8_t *sourceMac;
};

struct Event
{
  EventType type;

  void (*cleanup)(Event *);

  union
  {
    RawKeyEvent rawKeyEvt;
    RawBitmapEvent rawBitmapEvt;
    IdentifiableKeyEvent iKeyEvt;
    IdentifiableBitMapEvent iBitmapEvt;
  };
};

inline void cleanupRawKeyEvent(Event *event) { return; }
inline void cleanupRawBitmapEvent(Event *event) { free(event->rawBitmapEvt.bitMapData); }
inline void cleanupIdentifiableKeyEvent(Event *event) { return; }
inline void cleanupIdentifiableBitmapEvent(Event *event) { free(event->rawBitmapEvt.bitMapData); }

#endif