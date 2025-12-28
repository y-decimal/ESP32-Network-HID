#ifndef SYSTEMTASK_H
#define SYSTEMTASK_H

#include <FreeRTOS.h>
#include <modules/EventHandlerTask.h>
#include <modules/KeyScannerTask.h>
#include <modules/TaskParameters.h>
#include <submodules/ConfigManager.h>

extern QueueHandle_t priorityEventQueue;
extern QueueHandle_t eventQueue;

extern KeyScannerConfig keyCfg;

void initSystemTasks(ConfigManager *cfgManager) {

  priorityEventQueue = xQueueCreate(32, sizeof(Event));
  eventQueue = xQueueCreate(32, sizeof(Event));

  keyCfg = cfgManager->getKeyConfig();

  KeyScannerState *scannerState = new KeyScannerState;
  scannerState->bitMapSize = (keyCfg.rows * keyCfg.cols + 7) / 8;
  scannerState->bitMap = new uint8_t[scannerState->bitMapSize];
  memset(scannerState->bitMap, 0, scannerState->bitMapSize);

  KeyScannerParameters *keyParams = new KeyScannerParameters;
  keyParams->config = &keyCfg;
  keyParams->state = scannerState;

  xTaskCreatePinnedToCore(EventTask, "PriorityEventHandler",
                          STACK_PRIORITYEVENT, priorityEventQueue,
                          PRIORITY_PRIORITYEVENT, nullptr, CORE_PRIORITYEVENT);

  xTaskCreatePinnedToCore(keyScannerTask, "KeyScanner", STACK_KEYSCAN,
                          keyParams, PRIORITY_KEYSCAN, nullptr, CORE_KEYSCAN);
}

#endif