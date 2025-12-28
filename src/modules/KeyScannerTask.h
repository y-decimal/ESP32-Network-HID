#ifndef KEYSCANNERTASK_H
#define KEYSCANNERTASK_H

#include <modules/TaskParameters.h>
#include <shared/ConfigTypes.h>
#include <shared/EventTypes.h>
#include <submodules/KeyScanner.h>
#include <system/SystemConfig.h>

extern QueueHandle_t priorityEventQueue;

void keyEventCallback(uint16_t keyIndex, bool state) {
  KeyEvent keyEvent{keyIndex, state};
  Event event{};
  event.type = EventType::Key;
  event.key = keyEvent;
  xQueueSend(priorityEventQueue, &event, pdMS_TO_TICKS(10));
}

void keyScannerTask(void *arg) {
  KeyScannerParameters *params = static_cast<KeyScannerParameters *>(arg);

  if (!params) {
    printf("[KeyScannerTask]: Received invalid parameters, aborting");
    vTaskDelete(nullptr);
  }
  if (!params->config || !params->state) {
    printf("[KeyScannerTask]: Received invalid config or state, aborting");
    vTaskDelete(nullptr);
  }

  KeyScannerConfig *moduleCfg = params->config;
  KeyScannerState *state = params->state;

  // Copy only the values we need to local stack variables
  countType rows = moduleCfg->rows;
  countType cols = moduleCfg->cols;
  uint16_t refreshRate = moduleCfg->refreshRate;

  // Create appropriately-sized local arrays and copy pin data
  pinType rowPins[rows];
  pinType colPins[cols];

  for (countType i = 0; i < rows; i++) {
    rowPins[i] = moduleCfg->rowPins[i];
  }

  for (countType i = 0; i < cols; i++) {
    colPins[i] = moduleCfg->colPins[i];
  }

  KeyScanner keyScanner = KeyScanner(rowPins, colPins, rows, cols);

  keyScanner.registerOnKeyChangeCallback(keyEventCallback);

  TickType_t previousWakeTime = xTaskGetTickCount();
  TickType_t refreshRateToTicks = pdMS_TO_TICKS((1000 / refreshRate));

  while (true) {
    keyScanner.updateKeyState();
    keyScanner.copyPublishedBitmap(state->bitMap);
    xTaskDelayUntil(&previousWakeTime, refreshRateToTicks);
  }
}

#endif