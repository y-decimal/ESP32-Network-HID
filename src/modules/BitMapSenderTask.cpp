#include <system/TaskManager.h>

void TaskManager::bitMapSenderTask(void *arg) {
  BitMapSenderParameters *params = static_cast<BitMapSenderParameters *>(arg);

  if (!params) {
    printf("[BitMapSenderTask]: Received invalid parameters, aborting\n");
    vTaskDelete(nullptr);
  }
  if (!params->configManager || !params->state || !params->routing) {
    printf("[BitMapSenderTask]: Received invalid configManager/state/routing, "
           "aborting\n");
    vTaskDelete(nullptr);
  }

  // Get immutable local copy of config at task startup.
  // ConfigManager holds the live reference; this task operates only on its
  // snapshot.
  BitMapSenderConfig localConfig =
      params->configManager->getConfig<BitMapSenderConfig>();

  KeyScannerState *state = params->state;

  IBitMapRoutingStrategy *routing = params->routing;

  delete params;

  uint8_t bitMapSize = state->bitMapSize;
  uint16_t refreshRate = localConfig.getRefreshRate();

  uint8_t localBitmapCopy[bitMapSize];

  TickType_t refreshRateTicks = pdMS_TO_TICKS(1000 / refreshRate);

  TickType_t previousWakeTime = xTaskGetTickCount();

  while (true) {
    memcpy(localBitmapCopy, state->bitMap, state->bitMapSize);
    routing->routeBitMap(localBitmapCopy, state->bitMapSize);
    xTaskDelayUntil(&previousWakeTime, refreshRateTicks);
  }
}

// BitMapHandler Helper functions

void TaskManager::startBitmapSender() {
  BitMapSenderParameters bitMapParams;
  bitMapParams.configManager = &configManager;
  bitMapParams.state = keyScannerState;
  bitMapParams.routing = createRoutingStrategy();

  xTaskCreatePinnedToCore(bitMapSenderTask, "BitmapSender", STACK_BITMAP,
                          &bitMapParams, PRIORITY_BITMAP, nullptr, CORE_BITMAP);
}

void TaskManager::stopBitmapSender() {
  if (bitmapSenderHandle != nullptr)
    vTaskDelete(bitmapSenderHandle);
  bitmapSenderHandle == nullptr;
}
void TaskManager::restartBitmapSender() {
  if (bitmapSenderHandle != nullptr) {
    stopBitmapSender();
    startBitmapSender();
  }
}