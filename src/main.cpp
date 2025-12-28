#include <Arduino.h>
#include <tasks/SystemTask.h>

// temp local definitions for testing
uint8_t ROWPINS[2] = {9, 10};
uint8_t COLPINS[2] = {17, 18};
uint8_t rowCount = 2;
uint8_t colCount = 2;

QueueHandle_t priorityEventQueue;
QueueHandle_t eventQueue;
KeyScanner keyScanner = KeyScanner(ROWPINS, COLPINS, rowCount, colCount);

void setup() {
  Serial.begin(115200);
  initSystemTasks();
}

void loop() {
  KeyEvent event;
  if (xQueueReceive(priorityEventQueue, &event, pdMS_TO_TICKS(100)) == pdPASS) {
    printf("Key %d %s\n", event.keyIndex,
           (event.state ? "pressed" : "released"));
  }
}
