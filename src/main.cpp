#include <Arduino.h>
#include <WiFi.h>
#include <submodules/ConfigManager/ConfigManager.h>
#include <submodules/Storage/PreferencesStorage.h>
#include <system/TaskManager.h>

// temp local definitions for testing

#include <interfaces/IEspNow.h>
#include <submodules/Esp32Gpio.h>
#include <submodules/EspNow.h>

Esp32Gpio espGpio;
EspNow espNow; // Todo: implements IEspNow

PreferencesStorage prefStorage(CONFIG_MANAGER_NAMESPACE);
ConfigManager mainCfg(prefStorage);
TaskManager taskManager(mainCfg, espGpio, espNow); // Move outside setup()

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

void simulateConfig()
{

  if (mainCfg.loadConfig())
  {
    printf("Config loaded from flash\n");
    return;
  }

  printf("No config in flash, creating new config...\n");

  GlobalConfig gCfg;

  DeviceRole roles[1] = {DeviceRole::Keyboard};
  gCfg.setRoles(roles, 1);

  uint8_t mac[6];
  esp_efuse_mac_get_default(mac);
  MacAddress deviceMac = {mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]};
  gCfg.setMac(deviceMac);

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

void setup()
{
  Serial.begin(115200);
  delay(3000);
  printf("initializing...\n");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  simulateConfig();
  EventRegistry::registerHandler(EventType::Key, keyPrintCallback);
  KeyScannerConfig kCfg = mainCfg.getConfig<KeyScannerConfig>();
  printf("KeyScanner Config: %d rows, %d cols, refresh %d ms, bitmap interval "
         "%d ms\n",
         kCfg.getRowsCount(), kCfg.getColCount(), kCfg.getRefreshRate(),
         kCfg.getBitMapSendInterval());
  GlobalConfig gCfg = mainCfg.getConfig<GlobalConfig>();
  DeviceRole roles[static_cast<size_t>(DeviceRole::Count)] = {DeviceRole::Count};
  gCfg.getRoles(roles, static_cast<size_t>(DeviceRole::Count));
  printf("Device Role: %d\n", (uint8_t)roles[0]);
  uint8_t mac[6];
  gCfg.getMac(mac, 6);
  printf("Device MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
         mac[0], mac[1], mac[2],
         mac[3], mac[4], mac[5]);
  taskManager.start();
  printf("setup done\n");
}

void loop() {}
