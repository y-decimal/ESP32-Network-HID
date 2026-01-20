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

static Logger logger("Main");

static Esp32Gpio espGpio;
static EspNow espNow;
static PreferencesStorage prefStorage(CONFIG_MANAGER_NAMESPACE);

TaskManager::Platform platform = {espGpio, espNow, prefStorage};
static TaskManager taskManager(platform);

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

  if (memcmp(lastBitmap.data(), bitMapEvent.bitMapData, bitMapEvent.bitMapSize) != 0)
  {
    std::string debugStr = "Bitmap change: Size " + std::to_string(bitMapEvent.bitMapSize) + " Data:";
    for (size_t i = 0; i < bitMapEvent.bitMapSize; i++)
    {
      debugStr += " " + std::to_string(bitMapEvent.bitMapData[i]);
      ;
    }
    logger.info("%s", debugStr.c_str());

    lastBitmap.assign(bitMapEvent.bitMapData, bitMapEvent.bitMapData + bitMapEvent.bitMapSize);
  }
}

void setup()
{
  Serial.begin(115200);
  static ArduinoLogSink logSink;
  delay(3000);

  Logger::setGlobalSink(&logSink);
  Logger::setDefaultLogLevel(Logger::LogLevel::info);

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
