#include <FreeRTOS.h>
#include <modules/EventRegistry.h>
#include <queue.h>

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