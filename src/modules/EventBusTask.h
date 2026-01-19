#ifndef EVENTBUSTASK_H
#define EVENTBUSTASK_H

#include <interfaces/ITask.h>
#include <submodules/EventRegistry.h>
#include <queue.h>
#include <submodules/Logger.h>

static constexpr const char *EVENTBUSTASK_NAMESPACE = "EventBusTask";

class EventBusTask : public ITask
{
public:
  EventBusTask();
  ~EventBusTask();

  void start(TaskParameters params) override;
  void stop() override;
  void restart(TaskParameters params) override;

private:
  QueueHandle_t localQueue = nullptr;
  TaskHandle_t eventBusHandle = nullptr;
  static EventBusTask *instance;
  Logger internalLog = Logger("EventBus");

  static void taskEntry(void *param);

  static bool staticPushCallback(const Event &event);
  bool pushToQueue(const Event &event);
};

#endif