#ifndef BITMAPSENDERTASK_H
#define BITMAPSENDERTASK_H

#include <FreeRTOS.h>
#include <modules/TaskParameters.h>
#include <queue.h>
#include <shared/ConfigTypes.h>
#include <shared/DataTypes.h>
#include <task.h>

void bitMapSenderTask(void *arg) {
  BitMapSenderParameters *params = static_cast<BitMapSenderParameters *>(arg);

  if (!params) {
    printf("[BitMapSenderTask]: Received invalid parameters, aborting\n");
    vTaskDelete(nullptr);
  }
  if (!params->state || !params->callback) {
    printf("[BitMapSenderTask]: Received invalid state/callback, aborting\n");
    vTaskDelete(nullptr);
  }

  BitMapSenderConfig &moduleCfg = params->config;
  KeyScannerState *state = params->state;

  uint8_t bitMapSize = state->bitMapSize;
  uint16_t refreshRate = moduleCfg.getRefreshRate();

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