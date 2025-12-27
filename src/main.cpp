#include <Arduino.h>
#include <tasks/SystemTask.h>

QueueHandle_t keyEventQueue;
KeyScanner keyScanner = KeyScanner(ROWPINS, COLPINS);

void setup() {
  Serial.begin(115200);
  initSystemTasks();
}

void loop() {
  KeyEvent event;
  if (xQueueReceive(keyEventQueue, &event, pdMS_TO_TICKS(100)) == pdPASS) {
    printf("Key %d %s\n", event.keyIndex, (event.state ? "pressed" : "released"));
  }
}
