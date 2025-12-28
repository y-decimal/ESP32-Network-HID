#ifndef EVENTHANDLERTASK_H
#define EVENTHANDLERTASK_H

#include <FreeRTOS.h>
#include <queue.h>
#include <shared/EventTypes.h>
#include <submodules/EventRegistry.h>

void EventTask(void *arg) {
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

#endif