#include <Arduino.h>
#include <system/SystemTask.h>

// temp local definitions for testing

ConfigManager mainCfg;

// Define global variables from SystemTask.h
QueueHandle_t priorityEventQueue;
QueueHandle_t eventQueue;
KeyScannerConfig keyCfg;
BitMapSenderConfig bitmapCfg;

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

  if (mainCfg.loadConfig()) {
    printf("Config loaded from flash\n");
    return;
  }

  GlobalConfig gCfg;

  DeviceRole roles[1] = {DeviceRole::Keyboard};
  gCfg.setRoles(roles, 1);

  MacAddress mac = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
  gCfg.setMac(mac);

  mainCfg.setConfig(gCfg);

  KeyScannerConfig kCfg;

  uint8_t rowCount = 2;
  uint8_t colCount = 2;
  uint8_t ROWPINS[2] = {9, 10};
  uint8_t COLPINS[2] = {17, 18};
  uint16_t refreshRate = 1000;

  kCfg.rows = rowCount;
  kCfg.cols = colCount;
  kCfg.setRowPins(ROWPINS, 2);
  kCfg.setColPins(COLPINS, 2);
  kCfg.refreshRate = refreshRate;

  mainCfg.setConfig(kCfg);

  BitMapSenderConfig bCfg;

  uint16_t bitMapRefresh = 250;

  bCfg.refreshRate = bitMapRefresh;
  mainCfg.setConfig(bCfg);

  if (mainCfg.saveConfig())
    printf("Config saved to flash\n");
  else
    printf("Saving config failed\n");
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
