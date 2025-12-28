#ifndef BITMAPSENDERTASK_H
#define BITMAPSENDERTASK_H

#include <FreeRTOS.h>
#include <modules/TaskParameters.h>
#include <queue.h>
#include <shared/ConfigTypes.h>
#include <shared/DataTypes.h>
#include <task.h>

extern uint8_t *bitMapBuffer;

void bitMapSenderTask(void *arg) {
  BitMapSenderParameters *params = static_cast<BitMapSenderParameters *>(arg);

  if (!params) {
    printf("[KeyScannerTask]: Received invalid parameters, aborting");
    vTaskDelete(nullptr);
  }
  if (!params->config || !params->state || !params->callback) {
    printf(
        "[KeyScannerTask]: Received invalid config/state/callback, aborting");
    vTaskDelete(nullptr);
  }

  BitMapSenderConfig *moduleCfg = params->config;
  KeyScannerState *state = params->state;

  uint8_t bitMapSize = state->bitMapSize;
  uint16_t refreshRate = moduleCfg->refreshRate;

  uint8_t localBitmapCopy[bitMapSize];

  TickType_t refreshRateTicks = pdMS_TO_TICKS(1000 / refreshRate);

  TickType_t previousWakeTime = xTaskGetTickCount();

  while (true) {
    memcpy(localBitmapCopy, state->bitMap, state->bitMapSize);
    params->callback(localBitmapCopy, state->bitMapSize);
    xTaskDelayUntil(&previousWakeTime, refreshRateTicks);
  }
}

#endif