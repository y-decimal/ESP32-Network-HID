#pragma once
#include <modules/KeyScanner.h>
#include <shared/ConfigTypes.h>
#include <shared/EventTypes.h>
#include <system/SystemConfig.h>

extern QueueHandle_t priorityEventQueue;

void keyEventCallback(uint16_t keyIndex, bool state) {
  KeyEvent keyEvent{keyIndex, state};
  Event event{EventType::Key, keyEvent};
  xQueueSend(priorityEventQueue, &event, pdMS_TO_TICKS(10));
}

void keyScannerTask(void *arg) {
  KeyScannerConfig moduleCfg = *static_cast<KeyScannerConfig *>(arg);

  KeyScanner keyScanner = KeyScanner(moduleCfg.rowPins, moduleCfg.colPins,
                                     moduleCfg.rows, moduleCfg.cols);

  keyScanner.registerOnKeyChangeCallback(keyEventCallback);

  TickType_t previousWakeTime = xTaskGetTickCount();
  TickType_t refreshRateToTicks = pdMS_TO_TICKS((moduleCfg.refreshRate / 1000));

  while (true) {
    keyScanner.updateKeyState();
    xTaskDelayUntil(&previousWakeTime, pdMS_TO_TICKS(refreshRateToTicks));
  }
}
