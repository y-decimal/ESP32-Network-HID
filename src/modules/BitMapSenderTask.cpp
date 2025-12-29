#include <system/TaskManager.h>

void TaskManager::bitMapSenderTask(void *arg) {
  BitMapSenderParameters *params = static_cast<BitMapSenderParameters *>(arg);

  if (!params) {
    printf("[BitMapSenderTask]: Received invalid parameters, aborting\n");
    vTaskDelete(nullptr);
  }
  if (!params->configManager || !params->state || !params->callback) {
    printf("[BitMapSenderTask]: Received invalid configManager/state/callback, "
           "aborting\n");
    vTaskDelete(nullptr);
  }

  // Get immutable local copy of config at task startup.
  // ConfigManager holds the live reference; this task operates only on its
  // snapshot.
  BitMapSenderConfig localConfig =
      params->configManager->getConfig<BitMapSenderConfig>();
  KeyScannerState *state = params->state;

  uint8_t bitMapSize = state->bitMapSize;
  uint16_t refreshRate = localConfig.getRefreshRate();

  uint8_t localBitmapCopy[bitMapSize];

  TickType_t refreshRateTicks = pdMS_TO_TICKS(1000 / refreshRate);

  TickType_t previousWakeTime = xTaskGetTickCount();

  while (true) {
    memcpy(localBitmapCopy, state->bitMap, state->bitMapSize);
    params->callback(localBitmapCopy, state->bitMapSize);
    xTaskDelayUntil(&previousWakeTime, refreshRateTicks);
  }
}