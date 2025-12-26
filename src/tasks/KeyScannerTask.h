#include <modules/KeyScanner.h>
#include <system/SharedTypes.h>
#include <system/SystemConfig.h>

extern QueueHandle_t keyEventQueue;

void keyEventCallback(uint16_t keyIndex, bool state) {
  KeyEvent event{keyIndex, state};
  xQueueSend(keyEventQueue, &event, 0);
}

void keyScannerTask(void *arg) {
  KeyScanner *keyScanner = static_cast<KeyScanner *>(arg);

  keyScanner->registerOnKeyChangeCallback(keyEventCallback);

  TickType_t previousWakeTime = xTaskGetTickCount();

  while (true) {
    keyScanner->updateKeyState();
    xTaskDelayUntil(&previousWakeTime, PERIOD_KEYSCAN_MS);
  }
}
