#ifndef SYTEMTASK_H
#define SYSTEMTASK_H

#include <FreeRTOS.h>
#include <submodules/ConfigManager.h>
#include <modules/EventHandlerTask.h>
#include <modules/KeyScannerTask.h>

static QueueHandle_t priorityEventQueue;
static QueueHandle_t eventQueue;
static ConfigManager cfgManager;

void initSystemTasks() {
  priorityEventQueue = xQueueCreate(32, sizeof(Event));
  eventQueue = xQueueCreate(32, sizeof(Event));

  xTaskCreatePinnedToCore(EventTask, "PriorityEventHandler",
                          STACK_PRIORITYEVENT, priorityEventQueue,
                          PRIORITY_PRIORITYEVENT, nullptr, CORE_PRIORITYEVENT);

  xTaskCreatePinnedToCore(keyScannerTask, "KeyScanner", STACK_KEYSCAN,
                          (void *)&cfgManager.getKeyConfig(), PRIORITY_KEYSCAN, nullptr,
                          CORE_KEYSCAN);
}

#endif