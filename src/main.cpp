#include <Arduino.h>
#include <WiFi.h>
#include <submodules/ConfigManager/ConfigManager.h>
#include <submodules/Storage/PreferencesStorage.h>
#include <system/TaskManager.h>
#include <submodules/ArduinoLogSink.h>
#include <submodules/Logger.h>

// temp local definitions for testing

#include <interfaces/ITransport.h>
#include <submodules/Esp32Gpio.h>
#include <submodules/EspNowTransport.h>

Esp32Gpio espGpio;
EspNow espNow;

PreferencesStorage prefStorage(CONFIG_MANAGER_NAMESPACE);
ConfigManager mainCfg(&prefStorage);
TaskManager taskManager(mainCfg, espGpio, espNow); // Move outside setup()

ArduinoLogSink logSink;
Logger logger("Main");

void keyPrintCallback(const Event &event)
{
  RawKeyEvent keyEvent;
  if (event.type == EventType::IdKey)
    keyEvent = event.idKeyEvt.raw;
  else
    keyEvent = event.rawKeyEvt;

  uint8_t keyIndex = keyEvent.keyIndex;
  bool state = keyEvent.state;
  logger.info("Key event: Key %d %s", keyIndex, state ? "pressed" : "released");
}

void bitMapPrintCallback(const Event &event)
{
  static std::vector<uint8_t> lastBitmap = {0};

  RawBitmapEvent bitMapEvent;
  if (event.type == EventType::IdBitmap)
    bitMapEvent = event.idBitmapEvt.raw;
  else
    bitMapEvent = event.rawBitmapEvt;

  if (memcmp(lastBitmap.data(), bitMapEvent.bitMapData, bitMapEvent.bitMapSize) != 0)
  {
    std::string debugStr = "Bitmap change: Size " + std::to_string(bitMapEvent.bitMapSize) + " Data:";
    for (size_t i = 0; i < bitMapEvent.bitMapSize; i++)
    {
      debugStr += " " + std::to_string(bitMapEvent.bitMapData[i]);;
    }
    logger.info("%s", debugStr.c_str());
    
    lastBitmap.assign(bitMapEvent.bitMapData, bitMapEvent.bitMapData + bitMapEvent.bitMapSize);
  }
}

void simulateConfig()
{

  if (mainCfg.loadConfig())
  {
    logger.info("Config loaded from flash");
    return;
  }

  logger.info("No config in flash, creating new config...");
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
    logger.info("Config saved to flash");
  else
    logger.error("Saving config failed");
}

void setup()
{
  Serial.begin(115200);
  delay(3000);

  Logger::setGlobalSink(&logSink);
  Logger::setDefaultLogLevel(Logger::LogLevel::info);

  logger.info("initializing...");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  simulateConfig();

  EventRegistry::registerHandler(EventType::RawKey, keyPrintCallback);
  EventRegistry::registerHandler(EventType::RawBitmap, bitMapPrintCallback);
  EventRegistry::registerHandler(EventType::IdKey, keyPrintCallback);
  EventRegistry::registerHandler(EventType::IdBitmap, bitMapPrintCallback);

  KeyScannerConfig kCfg = mainCfg.getConfig<KeyScannerConfig>();
  logger.info("KeyScanner Config: %d rows, %d cols, refresh %d ms, bitmap interval %d ms",
         kCfg.getRowsCount(), kCfg.getColCount(), kCfg.getRefreshRate(),
         kCfg.getBitMapSendInterval());

  GlobalConfig gCfg = mainCfg.getConfig<GlobalConfig>();
  DeviceRole roles[static_cast<size_t>(DeviceRole::Count)] = {DeviceRole::Count};
  gCfg.getRoles(roles, static_cast<size_t>(DeviceRole::Count));
  logger.info("Device Role: %d", (uint8_t)roles[0]);
  uint8_t mac[6];
  gCfg.getMac(mac, 6);
  logger.info("Device MAC: %02x:%02x:%02x:%02x:%02x:%02x",
         mac[0], mac[1], mac[2],
         mac[3], mac[4], mac[5]);

  taskManager.start();

  logger.info("setup complete");
}

void loop() {}
