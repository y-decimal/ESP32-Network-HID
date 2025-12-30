#include <system/TaskManager.h>

void TaskManager::priorityEventTask(void *arg) {
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

void TaskManager::startPriorityEventHandler() {

  if (priorityEventHandle != nullptr)
    return;

  BaseType_t result = xTaskCreatePinnedToCore(
      priorityEventTask, "PriorityEventHandler", STACK_PRIORITYEVENT,
      highPrioEventQueue, PRIORITY_PRIORITYEVENT, &priorityEventHandle,
      CORE_PRIORITYEVENT);

  if (result != pdPASS) {
    priorityEventHandle = nullptr;
  }
}

void TaskManager::stopPriorityEventHandler() {
  if (priorityEventHandle == nullptr)
    return;
  vTaskDelete(priorityEventHandle);
  priorityEventHandle = nullptr;
}

void TaskManager::restartPriorityEventHandler() {
  if (priorityEventHandle != nullptr)
    stopPriorityEventHandler();
  startPriorityEventHandler();
}
