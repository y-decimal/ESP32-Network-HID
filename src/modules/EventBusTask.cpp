#include <modules/EventBusTask.h>


void EventBusTask::taskEntry(void *param) {
  QueueHandle_t queue = static_cast<QueueHandle_t>(param);
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

void EventBusTask::start(TaskParameters params) {

  if (eventBusHandle != nullptr)
    return;

  BaseType_t result = xTaskCreatePinnedToCore(
      EventBusTask::taskEntry, "EventBusHandler", params.stackSize, queue,
      params.priority, &eventBusHandle, params.coreAffinity);

  if (result != pdPASS) {
    eventBusHandle = nullptr;
  }
}

void EventBusTask::stop() {
  if (eventBusHandle == nullptr)
    return;
  vTaskDelete(eventBusHandle);
  eventBusHandle = nullptr;
}

void EventBusTask::restart(TaskParameters params) {
  if (eventBusHandle != nullptr)
    stop();
  start(params);
}
