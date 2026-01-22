#include <Arduino.h>
#include <WiFi.h>
#include <submodules/Config/ConfigManager.h>
#include <submodules/Storage/PreferencesStorage.h>
#include <system/TaskManager.h>
#include <submodules/ArduinoLogSink.h>
#include <submodules/Logger.h>

// temp local definitions for testing

#include <interfaces/ITransport.h>
#include <submodules/Esp32Gpio.h>
#include <submodules/EspNowTransport.h>

static Logger logger("Main");

static Esp32Gpio espGpio;
static EspNow espNow;
static PreferencesStorage prefStorage(CONFIG_MANAGER_NAMESPACE);

TaskManager::Platform platform = {espGpio, espNow, prefStorage};
static TaskManager taskManager(platform);

static void keyPrintCallback(const Event &event);
static void bitMapPrintCallback(const Event &event);
static void hidPrintCallback(const Event &event);

static void setKeyboardConfig();
static void setHostConfig();

void setup()
{
  Logger::setDefaultLogLevel(Logger::LogLevel::warn);
  Logger::setNamespaceLevel("Main", Logger::LogLevel::info);
  Logger::setNamespaceLevel(LOGGERTASK_NAMESPACE, Logger::LogLevel::warn);
  Logger::setNamespaceLevel(MASTERTASK_NAMESPACE, Logger::LogLevel::info);
  Logger::setNamespaceLevel(SLAVETASK_NAMESPACE, Logger::LogLevel::info);
  Logger::setNamespaceLevel("TransportProtocol.cpp", Logger::LogLevel::info);

  // setHostConfig();
  // setKeyboardConfig();

  logger.info("Starting setup...");
  Serial.begin(115200);
  logger.info("Serial initialized.");
  delay(3000);

  static ArduinoLogSink logSink;
  Logger::setGlobalSink(&logSink);

  logger.info("initializing...");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  EventRegistry::registerHandler(EventType::RawKey, keyPrintCallback);
  EventRegistry::registerHandler(EventType::RawBitmap, bitMapPrintCallback);
  EventRegistry::registerHandler(EventType::HidBitmap, hidPrintCallback);

  taskManager.start();

  logger.info("setup complete");
}

void loop() {}

static void keyPrintCallback(const Event &event)
{
  if (event.type != EventType::RawKey)
  {
    logger.warn("Received wrong event type");
    return;
  }

  RawKeyEvent keyEvent;
  keyEvent = event.rawKeyEvt;

  uint8_t keyIndex = keyEvent.keyIndex;
  bool state = keyEvent.state;

  ConfigManager &localCfgCopy = taskManager.getConfigManagerCopy();
  if (&localCfgCopy != nullptr)
  {
    uint8_t hidCode = localCfgCopy
                          .getConfig<KeyScannerConfig>()
                          .getHIDCodeForIndex(keyIndex);
    logger.info("Key event: Key Index %d HID Code 0x%02X %s",
                keyIndex, hidCode, state ? "pressed" : "released");
  }
  else
  {
    logger.info("Key event: Key %d %s", keyIndex, state ? "pressed" : "released");
  }
}

static void bitMapPrintCallback(const Event &event)
{
  static std::vector<uint8_t> lastBitmap = {0};

  RawBitmapEvent bitMapEvent;
  bitMapEvent = event.rawBitmapEvt;

  if (memcmp(lastBitmap.data(), bitMapEvent.bitMapData, bitMapEvent.bitmapSize) != 0)
  {
    std::string debugStr = "Bitmap change: Size " + std::to_string(bitMapEvent.bitmapSize) + " Data:";
    for (size_t i = 0; i < bitMapEvent.bitmapSize; i++)
    {
      debugStr += " " + std::to_string(bitMapEvent.bitMapData[i]);
    }
    logger.info("%s", debugStr.c_str());

    lastBitmap.assign(bitMapEvent.bitMapData, bitMapEvent.bitMapData + bitMapEvent.bitmapSize);
  }
}

static void hidPrintCallback(const Event &event)
{
  HidBitmapEvent hidEvent;
  hidEvent = event.hidBitmapEvt;

  std::string debugStr = "HID Bitmap: Size " + std::to_string(hidEvent.bitmapSize) + " Data:";
  for (size_t i = 0; i < hidEvent.bitmapSize; i++)
  {
    debugStr += " " + std::to_string(hidEvent.bitMapData[i]);
  }
  logger.info("%s", debugStr.c_str());
}

static void setKeyboardConfig()
{
  ConfigManager configManager(&prefStorage);

  configManager.clearAllConfigs();

  GlobalConfig globalConfig;

  GlobalConfig::DeviceModule modules[] = {GlobalConfig::DeviceModule::Keyscanner};
  globalConfig.setDeviceModules(modules, sizeof(modules) / sizeof(modules[0]));

  GlobalConfig::MacAddress mac = {};
  esp_base_mac_addr_get(mac);
  globalConfig.setMac(mac);

  globalConfig.setDeviceMode(GlobalConfig::DeviceMode::Slave);
  configManager.setConfig(globalConfig);

  KeyScannerConfig keyScannerConfig;

  uint8_t rowPins[2] = {9, 10};
  uint8_t colPins[2] = {17, 18};
  uint8_t localToHidMap[2 * 2] = {
      0x46, 0x7F, 0x7C, 0x7D // HID codes for the 4 keys: PrntScr, Mute, Copy, Paste
  }; // Note: Adjust size according to rowCount * colCount

  KeyScannerConfig::KeyCfgParams keyCfgParams;
  keyCfgParams.rowCount = 2;
  keyCfgParams.colCount = 2;
  keyCfgParams.rowPins = rowPins;
  keyCfgParams.colPins = colPins;
  keyCfgParams.refreshRate = 500;
  keyCfgParams.bitmapSendRate = 1;
  keyCfgParams.localToHidMap = localToHidMap;
  keyScannerConfig.setConfig(keyCfgParams);

  keyScannerConfig.setLocalToHidMap(localToHidMap, 4); // 2 rows * 2 cols = 4 keys

  std::vector<uint8_t> hidMap = keyScannerConfig.getLocalToHidMap();
  logger.debug("Returned KeyScanner HID Map:");
  logger.debug("Size: %d", hidMap.size());
  for (size_t i = 0; i < hidMap.size(); i++)
  {
    logger.debug("  Index %d: HID 0x%02X", i, hidMap[i]);
  }

  configManager.setConfig(keyScannerConfig);

  KeyScannerConfig localConfig = configManager.getConfig<KeyScannerConfig>();

  hidMap = localConfig.getLocalToHidMap();
  logger.debug("Returned ConfigManager HID Map:");
  logger.debug("Size: %d", hidMap.size());
  for (size_t i = 0; i < hidMap.size(); i++)
  {
    logger.debug("  Index %d: HID 0x%02X", i, hidMap[i]);
  }

  configManager.saveConfig();
}

static void setHostConfig()
{
  ConfigManager configManager(&prefStorage);

  configManager.clearAllConfigs();

  GlobalConfig globalConfig;

  GlobalConfig::MacAddress mac = {};
  esp_base_mac_addr_get(mac);
  globalConfig.setMac(mac);

  globalConfig.setDeviceMode(GlobalConfig::DeviceMode::Master);
  configManager.setConfig(globalConfig);

  configManager.saveConfig();
}
