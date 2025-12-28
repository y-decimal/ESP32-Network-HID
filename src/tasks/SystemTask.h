#ifndef SYTEMTASK_H
#define SYSTEMTASK_H

#include <FreeRTOS.h>
#include <tasks/EventHandlerTask.h>
#include <tasks/KeyScannerTask.h>

extern QueueHandle_t priorityEventQueue;
extern QueueHandle_t eventQueue;

extern KeyScanner keyScanner;

void initSystemTasks() {
  // Initialize KeyScanner Task
  priorityEventQueue = xQueueCreate(32, sizeof(Event));
  eventQueue = xQueueCreate(32, sizeof(Event));

  xTaskCreatePinnedToCore(EventTask, "PriorityEventHandler",
                          STACK_PRIORITYEVENT, &priorityEventQueue,
                          PRIORITY_PRIORITYEVENT, nullptr, CORE_PRIORITYEVENT);

  xTaskCreatePinnedToCore(keyScannerTask, "KeyScanner", STACK_KEYSCAN,
                          &keyScanner, PRIORITY_KEYSCAN, nullptr, CORE_KEYSCAN);
}

#endif