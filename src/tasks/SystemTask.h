#pragma once
#include <FreeRTOS.h>
#include <system/SharedTypes.h>
#include <tasks/KeyScannerTask.h>

extern QueueHandle_t keyEventQueue;

extern KeyScanner keyScanner;

void initSystemTasks() {
  // Initialize KeyScanner Task
  keyEventQueue = xQueueCreate(32, sizeof(KeyEvent));
  xTaskCreatePinnedToCore(keyScannerTask, "KeyScanner", STACK_KEYSCAN,
                          &keyScanner, PRIORITY_KEYSCAN, nullptr, CORE_KEYSCAN);
}