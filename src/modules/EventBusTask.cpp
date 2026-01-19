#include <modules/EventBusTask.h>

EventBusTask::EventBusTask()
{
  if (instance != nullptr)
    return;
  instance = this;
  localQueue = xQueueCreate(32, sizeof(Event));
  EventRegistry::registerPushCallback(staticPushCallback);
}

EventBusTask::~EventBusTask()
{
  if (localQueue != nullptr)
  {
    vQueueDelete(localQueue);
    localQueue = nullptr;
  }
  EventRegistry::clearPushCallback();
  instance = nullptr;
}

// Main task loop
void EventBusTask::taskEntry(void *param)
{
  EventBusTask* instance = static_cast<EventBusTask*>(param);
  Event event;

  while (true)
  {
    if (xQueueReceive(instance->localQueue, &event, portMAX_DELAY))
    {
      const auto handlers = EventRegistry::getHandler(event.type);
      for (auto callback : handlers)
        callback(event);
    }
  }
}

// Event pushing helper functions

bool EventBusTask::staticPushCallback(const Event &event)
{
  if (instance != nullptr)
  {
    return instance->pushToQueue(event);
  }
  return false;
}

bool EventBusTask::pushToQueue(const Event &event)
{
  if (localQueue != nullptr)
  {
    return xQueueSend(localQueue, &event, 0) == pdTRUE;
  }
  return false;
}

// Task lifecycle methods

void EventBusTask::start(TaskParameters params)
{

  if (eventBusHandle != nullptr)
    return;

  BaseType_t result = xTaskCreatePinnedToCore(
      EventBusTask::taskEntry, "EventBusHandler", params.stackSize, this,
      params.priority, &eventBusHandle, params.coreAffinity);

  if (result != pdPASS)
  {
    eventBusHandle = nullptr;
  }
}

void EventBusTask::stop()
{
  if (eventBusHandle == nullptr)
    return;
  vTaskDelete(eventBusHandle);
  eventBusHandle = nullptr;
}

void EventBusTask::restart(TaskParameters params)
{
  if (eventBusHandle != nullptr)
    stop();
  start(params);
}
