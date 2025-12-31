#include <submodules/EventRegistry.h>

void EventRegistry::registerHandler(EventType type, EventCallback callback) {
  handlers[(size_t)type].push_back(callback);
}

std::vector<EventRegistry::EventCallback>
EventRegistry::getHandler(EventType type) {
  return handlers[(size_t)type];
}

std::vector<EventRegistry::EventCallback>
    EventRegistry::handlers[(size_t)EventType::COUNT]{};

void EventRegistry::clearHandlers(EventType type) {
  handlers[(size_t)type].clear();
}