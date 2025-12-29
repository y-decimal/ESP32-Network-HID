#include <shared/EventTypes.h>
#include <submodules/KeyScanner.h>
#include <system/TaskManager.h>

static QueueHandle_t localEventQueueReference = nullptr;

void keyEventCallback(uint16_t keyIndex, bool state) {
  KeyEvent keyEvent{keyIndex, state};
  Event event{};
  event.type = EventType::Key;
  event.key = keyEvent;
  xQueueSend(localEventQueueReference, &event, pdMS_TO_TICKS(10));
}

void sendBitMapEvent(uint8_t bitMapSize, uint8_t *bitMap) {
  BitMapEvent bitMapEvent{};
  memcpy(bitMapEvent.bitMap, bitMap, bitMapSize);
  Event event{};
  event.type = EventType::BitMap;
  event.bitMap = bitMapEvent;
  xQueueSend(localEventQueueReference, &event, pdMS_TO_TICKS(10));
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

  // Copy only the values we need to local stack variables
  countType rows = localConfig.rows;
  countType cols = localConfig.cols;
  uint16_t refreshRate = localConfig.getRefreshRate();
  uint16_t bitMapRatio = localConfig.getBitMap();

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
    xTaskDelayUntil(&previousWakeTime, refreshRateToTicks);
  }
}

// KeyScanner helper functions

void TaskManager::startKeyScanner() {

  if (keyScannerHandle != nullptr)
    return;

  KeyScannerParameters keyParams;
  keyParams.configManager = &configManager;
  keyParams.eventQueueHandle = highPrioEventQueue;
  xTaskCreatePinnedToCore(keyScannerTask, "KeyScanner", STACK_KEYSCAN,
                          &keyParams, PRIORITY_KEYSCAN, &keyScannerHandle,
                          CORE_KEYSCAN);
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