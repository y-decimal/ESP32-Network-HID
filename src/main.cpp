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

void keyPrintCallback(const Event &event) {
  if (event.type != EventType::Key) {
    printf("Received wrong event type\n");
    return;
  }
  KeyEvent keyEvent = event.key;
  uint8_t keyIndex = keyEvent.keyIndex;
  bool state = keyEvent.state;
  printf("Key event: Key %d %s\n", keyIndex, state ? "pressed" : "released");
}

void setup() {
  Serial.begin(115200);
  delay(3000);
  printf("initializing...\n");
  EventRegistry::registerHandler(EventType::Key, keyPrintCallback);
  initSystemTasks();
  printf("setup done\n");
}

void loop() {
}
