#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <FreeRTOS.h>
#include <modules/EventBusTask.h>
#include <system/SystemConfig.h>

class TaskManager
{
public:
  TaskManager() : eventBusTask(eventBusQueue) {}; // Task classes are initialized here

  void start() {
    eventBusTask.start({STACK_EVENTBUS, PRIORITY_EVENTBUS, CORE_EVENTBUS});
  }

private:
  // Task classes are stored here
  QueueHandle_t eventBusQueue = xQueueCreate(32, sizeof(Event));
  EventBusTask eventBusTask;
};

#endif