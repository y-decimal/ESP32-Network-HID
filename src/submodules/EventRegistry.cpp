#include <submodules/EventRegistry.h>

void EventRegistry::registerHandler(EventType type, EventCallback callback) {
  // Add the callback to the vector of handlers for the specified event type
  handlers[(size_t)type].push_back(callback);
}

std::vector<EventRegistry::EventCallback>
EventRegistry::getHandler(EventType type) {
  // Return the vector of handlers for the specified event type
  return handlers[(size_t)type];
}

// Initialize the static member variable
std::vector<EventRegistry::EventCallback>
    EventRegistry::handlers[(size_t)EventType::COUNT]{};

void EventRegistry::clearHandlers(EventType type) {
  // Clears all registered handlers for a specific event type
  handlers[(size_t)type].clear();
}