#include <submodules/EventRegistry.h>
#include <mutex>

// Initialize static member variables
std::vector<EventRegistry::EventCallback> EventRegistry::handlers[(size_t)EventType::COUNT]{};
EventRegistry::PushCallback EventRegistry::pushCallback = nullptr;

void EventRegistry::registerHandler(EventType type, EventCallback callback)
{
  std::lock_guard<std::mutex> lock(mutex);
  // Add the callback to the vector of handlers for the specified event type
  handlers[(size_t)type].push_back(callback);
}

std::vector<EventRegistry::EventCallback>
EventRegistry::getHandler(EventType type)
{
  std::lock_guard<std::mutex> lock(mutex);
  // Return the vector of handlers for the specified event type
  return handlers[(size_t)type];
}

void EventRegistry::clearHandlers(EventType type)
{
  std::lock_guard<std::mutex> lock(mutex);
  // Clears all registered handlers for a specific event type
  handlers[(size_t)type].clear();
}

void EventRegistry::registerPushCallback(PushCallback cb)
{
  std::lock_guard<std::mutex> lock(mutex);
  pushCallback = cb;
}

void EventRegistry::clearPushCallback()
{
  std::lock_guard<std::mutex> lock(mutex);
  pushCallback = nullptr;
}

bool EventRegistry::pushEvent(const Event &event)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (pushCallback)
  {
    return pushCallback(event);
  }
  return false;
}