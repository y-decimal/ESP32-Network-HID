#include <submodules/EventRegistry.h>

EventRegistry::EventCallback EventRegistry::handlers[(size_t)EventType::COUNT] =
    {nullptr};

void EventRegistry::registerHandler(EventType type, EventCallback callback) {
  handlers[(size_t)type] = callback;
}

EventRegistry::EventCallback EventRegistry::getHandler(EventType type) {
  return handlers[(size_t)type];
}