#include <Arduino.h>
#include <modules/SystemTask.h>

// temp local definitions for testing

ConfigManager mainCfg;

// Define global variables from SystemTask.h
QueueHandle_t priorityEventQueue;
QueueHandle_t eventQueue;
KeyScannerConfig keyCfg;

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

void simulateConfig() {
  DeviceRole roles[1] = {DeviceRole::Keyboard};
  MacAddress mac = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
  GlobalConfig gCfg;
  memcpy(gCfg.roles, roles, 1);
  memcpy(gCfg.deviceMac, mac, 6);
  mainCfg.setGlobalConfig(gCfg);

  uint8_t ROWPINS[2] = {9, 10};
  uint8_t COLPINS[2] = {17, 18};
  uint8_t rowCount = 2;
  uint8_t colCount = 2;
  uint16_t refreshRate = 1000;
  KeyScannerConfig kCfg;
  memcpy(kCfg.rowPins, ROWPINS, 2);
  memcpy(kCfg.colPins, COLPINS, 2);
  kCfg.rows = rowCount;
  kCfg.cols = colCount;
  kCfg.refreshRate = 1000;

  mainCfg.setGlobalConfig(gCfg);
  mainCfg.setKeyConfig(kCfg);
}

void setup() {
  Serial.begin(115200);
  delay(3000);
  printf("initializing...\n");
  simulateConfig();
  EventRegistry::registerHandler(EventType::Key, keyPrintCallback);
  initSystemTasks(&mainCfg);
  printf("setup done\n");
}

void loop() {}
