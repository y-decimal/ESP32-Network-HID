#ifndef KEYSCANNERTASK_H
#define KEYSCANNERTASK_H

#include <modules/TaskParameters.h>
#include <shared/ConfigTypes.h>
#include <shared/EventTypes.h>
#include <submodules/ConfigManager.h>
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
    printf("[KeyScannerTask]: Received invalid parameters, aborting\n");
    vTaskDelete(nullptr);
  }
  if (!params->configManager || !params->state) {
    printf("[KeyScannerTask]: Received invalid configManager or state, "
           "aborting\n");
    vTaskDelete(nullptr);
  }

  // Get immutable local copy of config at task startup.
  // ConfigManager holds the live reference; this task operates only on its
  // snapshot.
  KeyScannerConfig localConfig =
      params->configManager->getConfig<KeyScannerConfig>();
  KeyScannerState *state = params->state;

  // Copy only the values we need to local stack variables
  countType rows = localConfig.rows;
  countType cols = localConfig.cols;
  uint16_t refreshRate = localConfig.getRefreshRate();

  // Create appropriately-sized local arrays and copy pin data
  pinType rowPins[rows];
  pinType colPins[cols];

  for (countType i = 0; i < rows; i++) {
    rowPins[i] = localConfig.rowPins[i];
  }

  for (countType i = 0; i < cols; i++) {
    colPins[i] = localConfig.colPins[i];
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