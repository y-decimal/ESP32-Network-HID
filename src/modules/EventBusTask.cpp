#include <system/TaskManager.h>

void TaskManager::eventBusTask(void *arg) {
  QueueHandle_t queue = static_cast<QueueHandle_t>(arg);
  Event event;

  while (true) {
    if (xQueueReceive(queue, &event, portMAX_DELAY)) {
      const auto handlers = EventRegistry::getHandler(event.type);
      for (auto callback : handlers)
        callback(event);
    }
  }
}

// EventHandler helper functions

void TaskManager::startEventBus() {

  if (eventBusHandle != nullptr)
    return;

  BaseType_t result = xTaskCreatePinnedToCore(
      eventBusTask, "EventBusHandler", STACK_EVENTBUS, eventBusQueue,
      PRIORITY_EVENTBUS, &eventBusHandle, CORE_EVENTBUS);

  if (result != pdPASS) {
    eventBusHandle = nullptr;
  }
}

void TaskManager::stopEventBus() {
  if (eventBusHandle == nullptr)
    return;
  vTaskDelete(eventBusHandle);
  eventBusHandle = nullptr;
}

void TaskManager::restartEventBus() {
  if (eventBusHandle != nullptr)
    stopEventBus();
  startEventBus();
}
