#include <system/TaskManager.h>

void TaskManager::priorityEventTask(void *arg) {
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

void TaskManager::startPriorityEventHandler() {

  if (priorityEventHandle != nullptr)
    return;

  xTaskCreatePinnedToCore(priorityEventTask, "PriorityEventHandler",
                          STACK_PRIORITYEVENT, highPrioEventQueue,
                          PRIORITY_PRIORITYEVENT, &priorityEventHandle,
                          CORE_PRIORITYEVENT);
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
