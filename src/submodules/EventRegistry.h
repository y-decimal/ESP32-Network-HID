#ifndef EVENTREGISTRY_H
#define EVENTREGISTRY_H

#include <cstring>
#include <shared/EventTypes.h>
#include <vector>
/**
 * @brief Central registry for event handlers.
 *
 * EventRegistry allows registration and retrieval of event
 * handler callbacks for different event types. Handlers can be
 * registered to respond to specific events, and multiple handlers
 * can be associated with each event type.
 */
class EventRegistry {
public:
  /// @brief Type definition for event handler callbacks.
  using EventCallback = void (*)(const Event &);

  /**
   * @brief Register an event handler for a specific event type.
   * @param type The type of event to register the handler for.
   * @param cb The callback function to be invoked when the event occurs.
   */
  static void registerHandler(EventType type, EventCallback cb);

  /**
   * @brief Retrieve the list of registered handlers for a specific event type.
   * @param type The type of event to retrieve handlers for.
   * @return A vector of callback functions registered for the event type.
   */
  static std::vector<EventCallback> getHandler(EventType type);

  /**
   * @brief Clear all registered handlers for a specific event type.
   * @param type The type of event to clear handlers for.
   */
  static void clearHandlers(EventType type);

  static void registerPushCallback(EventCallback cb);

  static void clearPushCallback();

  static bool pushEvent(const Event &event);


private:
  static std::vector<EventCallback> handlers[(size_t)EventType::COUNT];
  static EventCallback pushCallback;
};

#endif