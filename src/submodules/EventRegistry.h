#ifndef EVENTREGISTRY_H
#define EVENTREGISTRY_H

#include <cstring>
#include <shared/EventTypes.h>

class EventRegistry {
public:
  using EventCallback = void (*)(const Event &);
  static void registerHandler(EventType type, EventCallback cb);
  static EventCallback getHandler(EventType type);

private:
  static EventCallback handlers[(size_t)EventType::COUNT];
};

#endif