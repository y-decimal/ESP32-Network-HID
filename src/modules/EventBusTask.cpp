#include <modules/EventBusTask.h>

EventBusTask::EventBusTask()
{
  internalLog.setMode(Logger::LogMode::Global);
  internalLog.info("Creating EventBusTask");
  if (instance != nullptr)
  {
    internalLog.error("EventBusTask instance already exists!");
    return;
  }
  instance = this;
  localQueue = xQueueCreate(32, sizeof(Event));
  EventRegistry::registerPushCallback(staticPushCallback);
}

EventBusTask::~EventBusTask()
{
  internalLog.info("Destroying EventBusTask");
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
  EventBusTask *instance = static_cast<EventBusTask *>(param);
  Event event;

  while (true)
  {
    if (xQueueReceive(instance->localQueue, &event, portMAX_DELAY))
    {
      instance->internalLog.debug("Processing event of type %d", static_cast<uint8_t>(event.type));
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
  internalLog.info("Starting EventBusTask");
  if (eventBusHandle != nullptr)
  {
    internalLog.warn("EventBusTask already running");
    return;
  }
  BaseType_t result = xTaskCreatePinnedToCore(
      EventBusTask::taskEntry, "EventBusHandler", params.stackSize, this,
      params.priority, &eventBusHandle, params.coreAffinity);

  if (result != pdPASS)
  {
    eventBusHandle = nullptr;
    internalLog.error("Failed to create EventBusTask");
  }
}

void EventBusTask::stop()
{
  internalLog.info("Stopping EventBusTask");
  if (eventBusHandle == nullptr) {
    internalLog.info("Stop called but EventBusTask is not running");
    return;
  }
  vTaskDelete(eventBusHandle);
  eventBusHandle = nullptr;
}

void EventBusTask::restart(TaskParameters params)
{
  internalLog.info("Restarting EventBusTask");
  if (eventBusHandle != nullptr)
    stop();
  start(params);
}
