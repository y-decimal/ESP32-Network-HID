#ifndef EVENTBUSTASK_H
#define EVENTBUSTASK_H

#include <interfaces/ITask.h>
#include <submodules/EventRegistry.h>
#include <FreeRTOS.h>
#include <queue.h>

class EventBusTask : public ITask
{
public:
  EventBusTask(QueueHandle_t eventQueue) : queue(eventQueue) {};
  void start(TaskParameters params) override;
  void stop() override;
  void restart(TaskParameters params) override;

private:
  static constexpr uint32_t STACK_EVENTBUS = 4096;
  static constexpr UBaseType_t PRIORITY_EVENTBUS = 5;
  static constexpr BaseType_t CORE_EVENTBUS = 1;

  QueueHandle_t queue;
  TaskHandle_t eventBusHandle = nullptr;
  static void taskEntry(void *param);
  void TaskMain();
};
#endif