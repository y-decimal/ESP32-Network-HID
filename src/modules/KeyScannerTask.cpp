#include <shared/EventTypes.h>
#include <submodules/KeyScanner.h>
#include <system/TaskManager.h>

static QueueHandle_t localEventQueueReference = nullptr;

void keyEventCallback(uint16_t keyIndex, bool state) {
  KeyEvent keyEvent{keyIndex, state};
  Event event{};
  event.type = EventType::Key;
  event.key = keyEvent;
  event.cleanup = cleanupKeyEvent;
  if (xQueueSend(localEventQueueReference, &event, pdMS_TO_TICKS(10)) != pdTRUE)
    printf("[KeyScanner]: Could not push key event to queue\n");
}

void sendBitMapEvent(uint8_t bitMapSize, uint8_t *bitMap) {
  BitMapEvent bitMapEvent{};
  bitMapEvent.bitMapSize = bitMapSize;
  bitMapEvent.bitMapData = static_cast<uint8_t *>(malloc(bitMapSize));
  memcpy(bitMapEvent.bitMapData, bitMap, bitMapSize);

  Event event{};
  event.type = EventType::BitMap;
  event.bitMap = bitMapEvent;
  event.cleanup = cleanupBitmapEvent;

  if (xQueueSend(localEventQueueReference, &event, pdMS_TO_TICKS(10)) != pdTRUE)
    printf("[KeyScanner]: Could not push bitmap event to queue\n");
}

void TaskManager::keyScannerTask(void *arg) {
  KeyScannerParameters *params = static_cast<KeyScannerParameters *>(arg);

  if (!params) {
    printf("[KeyScannerTask]: Received invalid parameters, aborting\n");
    vTaskDelete(nullptr);
  }
  if (!params->configManager) {
    printf("[KeyScannerTask]: Received invalid configManager, aborting\n");
    vTaskDelete(nullptr);
  }

  localEventQueueReference = params->eventQueueHandle;

  // Get immutable local copy of config at task startup.
  // ConfigManager holds the live reference; this task operates only on its
  // snapshot.
  KeyScannerConfig localConfig =
      params->configManager->getConfig<KeyScannerConfig>();

  delete params;

  // Store pin vectors locally so their data() pointers remain valid
  pinType rowPins = localConfig.getRowPins();
  pinType colPins = localConfig.getColPins();

  KeyScanner keyScanner = KeyScanner(
      rowPins.data(), colPins.data(),
      localConfig.getRowsCount(), localConfig.getColCount());

  keyScanner.registerOnKeyChangeCallback(keyEventCallback);

  TickType_t previousWakeTime = xTaskGetTickCount();
  TickType_t refreshRateToTicks =
      pdMS_TO_TICKS((1000 / localConfig.getRefreshRate()));

  // Calculate bitmap send interval in loops based on frequency
  // bitMapSendInterval is now in Hz (frequency), so convert to loop count
  uint16_t bitMapLoopInterval =
      localConfig.getRefreshRate() / localConfig.getBitMapSendInterval();
  if (bitMapLoopInterval == 0)
    bitMapLoopInterval = 1;  // Minimum 1 loop if freq > refresh rate

  uint16_t loopsSinceLastBitMap = 0;
  std::vector<uint8_t> localBitmap;
  localBitmap.assign(0, localConfig.getBitmapSize());

  while (true) {
    loopsSinceLastBitMap++;
    keyScanner.updateKeyState();
    if (loopsSinceLastBitMap >= bitMapLoopInterval) {
      keyScanner.copyPublishedBitmap(localBitmap.data(), localBitmap.size());
      uint8_t bitMapSize = static_cast<uint8_t>(keyScanner.getBitMapSize());
      sendBitMapEvent(bitMapSize, localBitmap.data());
      loopsSinceLastBitMap = 0;
    }
    xTaskDelayUntil(&previousWakeTime, refreshRateToTicks);
  }
}

// KeyScanner helper functions

void TaskManager::startKeyScanner() {

  KeyScannerParameters *keyParams = new KeyScannerParameters();
  keyParams->configManager = &configManager;
  keyParams->eventQueueHandle = highPrioEventQueue;
  BaseType_t result = xTaskCreatePinnedToCore(
      keyScannerTask, "KeyScanner", STACK_KEYSCAN, keyParams, PRIORITY_KEYSCAN,
      &keyScannerHandle, CORE_KEYSCAN);
  if (result != pdPASS) {
    delete keyParams;
    keyScannerHandle = nullptr;
  }
}

void TaskManager::stopKeyScanner() {
  if (keyScannerHandle == nullptr)
    return;
  vTaskDelete(keyScannerHandle);
  keyScannerHandle = nullptr;
}
void TaskManager::restartKeyScanner() {
  if (keyScannerHandle != nullptr)
    stopKeyScanner();
  startKeyScanner();
}