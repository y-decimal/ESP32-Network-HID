#include <Arduino.h>
#include <submodules/ConfigManager/ConfigManager.h>
#include <submodules/Storage/PreferencesStorage.h>
#include <system/TaskManager.h>
#include <submodules/ArduinoLogSink.h>
#include <submodules/Logger.h>

// temp local definitions for testing

PreferencesStorage prefStorage(CONFIG_MANAGER_NAMESPACE);
ConfigManager mainCfg(&prefStorage);
TaskManager taskManager(mainCfg); // Move outside setup()

ArduinoLogSink logSink;

void keyPrintCallback(const Event &event)
{
  if (event.type != EventType::Key)
  {
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

  printf("No config in flash, creating new config...\n");

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
  uint16_t bitMapSendFrequency = 1;

  KeyScannerConfig::KeyCfgParams cfgParams = {
      rowCount, colCount, ROWPINS, COLPINS, refreshRate, bitMapSendFrequency};

  kCfg.setConfig(cfgParams);

  mainCfg.setConfig(kCfg);

  if (mainCfg.saveConfig())
    printf("Config saved to flash\n");
  else
    printf("Saving config failed\n");
}

void setup() {
  Serial.begin(115200);
  delay(3000);
  Logger::setGlobalSink(&logSink);
  printf("initializing...\n");
  simulateConfig();
  EventRegistry::registerHandler(EventType::Key, keyPrintCallback);
  KeyScannerConfig kCfg = mainCfg.getConfig<KeyScannerConfig>();
  printf("KeyScanner Config: %d rows, %d cols, refresh %d ms, bitmap interval "
         "%d ms\n",
         kCfg.getRowsCount(), kCfg.getColCount(), kCfg.getRefreshRate(),
         kCfg.getBitMapSendInterval());
  taskManager.start();
  printf("setup done\n");
}

void loop() {}
