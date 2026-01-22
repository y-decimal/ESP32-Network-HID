#ifndef EVENTTYPES_H
#define EVENTTYPES_H
#include <cstdlib>
#include <stdint.h>

enum class EventType : uint8_t
{
  RawKey,
  RawBitmap,
  HidBitmap,
  ConfigUpdate,
  COUNT
};

struct RawKeyEvent
{
  uint16_t keyIndex;
  bool state;
};

struct RawBitmapEvent
{
  uint8_t bitmapSize;
  uint8_t *bitMapData;
};

struct HidBitmapEvent
{
  uint8_t bitmapSize;
  uint8_t *bitMapData;
};

struct Event
{
  EventType type;

  void (*cleanup)(Event *);

  union
  {
    RawKeyEvent rawKeyEvt;
    RawBitmapEvent rawBitmapEvt;
    HidBitmapEvent hidBitmapEvt;
  };
};

inline void cleanupRawKeyEvent(Event *event) { return; }
inline void cleanupRawBitmapEvent(Event *event) { free(event->rawBitmapEvt.bitMapData); }
inline void cleanupHidBitmapEvent(Event *event) { free(event->hidBitmapEvt.bitMapData); }

#endif