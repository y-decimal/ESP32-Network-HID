#include <modules/KeyScanner.h>
#include <system/SystemConfig.h>

void keyScannerTask(void *arg) {
  KeyScanner *keyScanner = static_cast<KeyScanner *>(arg);

  TickType_t previousWakeTime = xTaskGetTickCount();

  while (true) {
    keyScanner->updateKeyState();
    xTaskDelayUntil(&previousWakeTime, PERIOD_KEYSCAN_MS);
  }
}