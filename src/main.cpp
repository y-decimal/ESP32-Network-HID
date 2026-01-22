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
static void setKeyboardConfig();
static void setHostConfig();

void setup()
{
  Logger::setNamespaceLevel(LOGGERTASK_NAMESPACE, Logger::LogLevel::warn);
  Logger::setDefaultLogLevel(Logger::LogLevel::info);

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
  EventRegistry::registerHandler(EventType::IdKey, keyPrintCallback);
  EventRegistry::registerHandler(EventType::IdBitmap, bitMapPrintCallback);

  taskManager.start();

  logger.info("setup complete");
}

void loop() {}

static void keyPrintCallback(const Event &event)
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

static void bitMapPrintCallback(const Event &event)
{
  static std::vector<uint8_t> lastBitmap = {0};

  RawBitmapEvent bitMapEvent;
  if (event.type == EventType::IdBitmap)
    bitMapEvent = event.idBitmapEvt.raw;
  else
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
  keyScannerConfig.setRefreshRate(1000);
  keyScannerConfig.setBitmapSendFrequency(10);
  uint8_t rowPins[2] = {9, 10};
  uint8_t colPins[2] = {17, 18};
  keyScannerConfig.setPins(rowPins, 2, colPins, 2);

  configManager.setConfig(keyScannerConfig);

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
