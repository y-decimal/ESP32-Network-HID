#ifndef SYTEMTASK_H
#define SYSTEMTASK_H

#include <FreeRTOS.h>
#include <shared/EventTypes.h>
#include <tasks/KeyScannerTask.h>

extern QueueHandle_t keyEventQueue;

extern KeyScanner keyScanner;

void initSystemTasks() {
  // Initialize KeyScanner Task
  keyEventQueue = xQueueCreate(32, sizeof(KeyEvent));
  xTaskCreatePinnedToCore(keyScannerTask, "KeyScanner", STACK_KEYSCAN,
                          &keyScanner, PRIORITY_KEYSCAN, nullptr, CORE_KEYSCAN);
}

#endif