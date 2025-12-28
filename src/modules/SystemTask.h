#ifndef SYTEMTASK_H
#define SYSTEMTASK_H

#include <FreeRTOS.h>
#include <modules/EventHandlerTask.h>
#include <modules/KeyScannerTask.h>
#include <submodules/ConfigManager.h>

QueueHandle_t priorityEventQueue;
QueueHandle_t eventQueue;
ConfigManager cfgManager;

void initSystemTasks() {
  priorityEventQueue = xQueueCreate(32, sizeof(Event));
  eventQueue = xQueueCreate(32, sizeof(Event));

  KeyScannerConfig keyCfg = cfgManager.getKeyConfig();

  xTaskCreatePinnedToCore(EventTask, "PriorityEventHandler",
                          STACK_PRIORITYEVENT, priorityEventQueue,
                          PRIORITY_PRIORITYEVENT, nullptr, CORE_PRIORITYEVENT);

  xTaskCreatePinnedToCore(keyScannerTask, "KeyScanner", STACK_KEYSCAN, &keyCfg,
                          PRIORITY_KEYSCAN, nullptr, CORE_KEYSCAN);
}

#endif