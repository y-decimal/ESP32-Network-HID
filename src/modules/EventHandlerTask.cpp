#include <system/TaskManager.h>

void TaskManager::eventHandlerTask(void *arg) {
  QueueHandle_t queue = static_cast<QueueHandle_t>(arg);
  Event event;

  while (true) {
    if (xQueueReceive(queue, &event, portMAX_DELAY)) {
      auto handler = EventRegistry::getHandler(event.type);
      if (handler)
        handler(event);
    }
  }
}

// EventHandler helper functions

void TaskManager::startEventHandler() {

  if (eventManagerHandle != nullptr)
    return;

  registerEventCallbacks(); // sets the correct callbacks based on whether the
                            // device has the master role

  xTaskCreatePinnedToCore(eventHandlerTask, "PriorityEventHandler",
                          STACK_PRIORITYEVENT, highPrioEventQueue,
                          PRIORITY_PRIORITYEVENT, &eventManagerHandle,
                          CORE_PRIORITYEVENT);
}

void TaskManager::stopEventHandler() {
  if (eventManagerHandle == nullptr)
    return;
  vTaskDelete(eventManagerHandle);
  eventManagerHandle = nullptr;
}

void TaskManager::restartEventHandler() {
  if (eventManagerHandle != nullptr)
    stopEventHandler();
  startEventHandler();
}
