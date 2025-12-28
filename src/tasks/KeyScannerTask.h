#pragma once
#include <modules/KeyScanner.h>
#include <shared/CommTypes.h>
#include <system/SystemConfig.h>

extern QueueHandle_t EventQueue;

void keyEventCallback(uint16_t keyIndex, bool state) {
  KeyEvent event{keyIndex, state};
  xQueueSend(EventQueue, &event, pdMS_TO_TICKS(10));
}

void keyScannerTask(void *arg) {
  KeyScanner *keyScanner = static_cast<KeyScanner *>(arg);

  keyScanner->registerOnKeyChangeCallback(keyEventCallback);

  TickType_t previousWakeTime = xTaskGetTickCount();

  while (true) {
    keyScanner->updateKeyState();
    xTaskDelayUntil(&previousWakeTime, pdMS_TO_TICKS(PERIOD_KEYSCAN_MS));
  }
}
