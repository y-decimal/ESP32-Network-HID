#ifndef EVENTREGISTRY_H
#define EVENTREGISTRY_H

#include <cstring>
#include <shared/EventTypes.h>
#include <vector>

class EventRegistry {
public:
  using EventCallback = void (*)(const Event &);
  static void registerHandler(EventType type, EventCallback cb);
  static std::vector<EventCallback> getHandler(EventType type);
  static void clearHandlers(EventType type);

private:
  static std::vector<EventCallback> handlers[(size_t)EventType::COUNT];
};

#endif