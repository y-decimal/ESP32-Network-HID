#ifndef SYSTEMTASK_H
#define SYSTEMTASK_H

#include <FreeRTOS.h>
#include <cstring>
#include <modules/BitMapSenderTask.h>
#include <modules/EventHandlerTask.h>
#include <modules/KeyScannerTask.h>
#include <modules/TaskParameters.h>
#include <submodules/ConfigManager.h>
#include <system/SystemConfig.h>

extern QueueHandle_t priorityEventQueue;
extern QueueHandle_t eventQueue;

extern KeyScannerConfig keyCfg;
extern BitMapSenderConfig bitMapCfg;

inline void initSystemTasks(ConfigManager *cfgManager) {

  priorityEventQueue = xQueueCreate(32, sizeof(Event));
  configASSERT(priorityEventQueue != NULL);
  eventQueue = xQueueCreate(32, sizeof(Event));
  configASSERT(eventQueue != NULL);

  keyCfg = cfgManager->getConfig<KeyScannerConfig>();
  bitMapCfg = cfgManager->getConfig<BitMapSenderConfig>();

  static KeyScannerState scannerState;
  // Free previously allocated bitmap (if any) before allocating a new one
  if (scannerState.bitMap != nullptr) {
    delete[] scannerState.bitMap;
    scannerState.bitMap = nullptr;
  }
  scannerState.bitMapSize = (keyCfg.rows * keyCfg.cols + 7) / 8;
  scannerState.bitMap = new uint8_t[scannerState.bitMapSize];
  memset(scannerState.bitMap, 0, scannerState.bitMapSize);

  static KeyScannerParameters keyParams;
  keyParams.config = &keyCfg;
  keyParams.state = &scannerState;

  static BitMapSenderParameters bitmapParams;
  bitmapParams.config = &bitMapCfg;
  bitmapParams.state = &scannerState;

  xTaskCreatePinnedToCore(EventTask, "PriorityEventHandler",
                          STACK_PRIORITYEVENT, priorityEventQueue,
                          PRIORITY_PRIORITYEVENT, nullptr, CORE_PRIORITYEVENT);

  xTaskCreatePinnedToCore(keyScannerTask, "KeyScanner", STACK_KEYSCAN,
                          &keyParams, PRIORITY_KEYSCAN, nullptr, CORE_KEYSCAN);

  xTaskCreatePinnedToCore(bitMapSenderTask, "BitmapSender", STACK_BITMAP,
                          &bitmapParams, PRIORITY_BITMAP, nullptr, CORE_BITMAP);
}

#endif