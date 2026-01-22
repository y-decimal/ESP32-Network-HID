#include <modules/KeyScannerTask.h>
#include <submodules/Logger.h>

static Logger log(KEYSCANNER_NAMESPACE);

// Initialize static member variable
KeyScannerTask *KeyScannerTask::instance = nullptr;

KeyScannerTask::KeyScannerTask(ConfigManager &configManager, IGpio &gpio)
    : configManagerRef(&configManager),
      gpioRef(&gpio)
{
  if (instance != nullptr)
  {
    log.warn("KeyScannerTask instance already exists, replacing");
    delete instance;
  }
  instance = this;
}

KeyScannerTask::~KeyScannerTask()
{
  stop();
  instance = nullptr;
  // more later if needed
}

void KeyScannerTask::keyEventCallback(uint16_t keyIndex, bool state)
{
  RawKeyEvent rKeyEvent{keyIndex, state};
  Event event{};
  event.type = EventType::RawKey;
  event.rawKeyEvt = rKeyEvent;
  event.cleanup = cleanupRawKeyEvent;
  if (!EventRegistry::pushEvent(event))
  {
    log.error("Failed to push key event to EventRegistry");
    event.cleanup(&event);
  }
  else
  {
    log.debug("Pushed key event: Key %d %s", keyIndex, state ? "pressed" : "released");
  }
}

void KeyScannerTask::sendBitMapEvent(uint8_t bitmapSize, uint8_t *bitMap)
{
  RawBitmapEvent rBitmapEvent{};
  rBitmapEvent.bitmapSize = bitmapSize;
  rBitmapEvent.bitMapData = static_cast<uint8_t *>(malloc(bitmapSize));
  memcpy(rBitmapEvent.bitMapData, bitMap, bitmapSize);

  Event event{};
  event.type = EventType::RawBitmap;
  event.rawBitmapEvt = rBitmapEvent;
  event.cleanup = cleanupRawBitmapEvent;

  if (!EventRegistry::pushEvent(event))
  {
    log.error("Failed to push bitmap event to EventRegistry");
    event.cleanup(&event);
  }
  else
  {
    log.debug("Pushed bitmap event of size %d", bitmapSize);
  }
}

void KeyScannerTask::taskEntry(void *arg)
{
  KeyScannerTask *task = static_cast<KeyScannerTask *>(arg);

  if (!task)
  {
    log.error("Received invalid parameters, aborting");
    vTaskDelete(nullptr);
  }

  // Get immutable local copy of config at task startup.
  // ConfigManager holds the live reference; this task operates only on its
  // snapshot.
  KeyScannerConfig localConfig =
      task->configManagerRef->getConfig<KeyScannerConfig>();

  IGpio &gpio = *task->gpioRef;

  // Store pin vectors locally so their data() pointers remain valid
  pinType rowPins = localConfig.getRowPins();
  pinType colPins = localConfig.getColPins();

  KeyScanner keyScanner =
      KeyScanner(gpio, rowPins.data(), colPins.data(),
                 localConfig.getRowsCount(), localConfig.getColCount());
  log.debug("Initialized KeyScanner with %d rows and %d columns",
            localConfig.getRowsCount(), localConfig.getColCount());

  keyScanner.registerOnKeyChangeCallback(keyEventCallback);
  log.debug("Registered key event callback with KeyScanner");

  std::vector<uint8_t> localBitmap;
  localBitmap.assign(localConfig.getBitmapSize(), 0);

  const uint32_t keyScanInterval = 1000000 / localConfig.getRefreshRate();
  const uint32_t bitmapSendInterval = 1000000 / localConfig.getBitmapSendRate();

  uint64_t lastScanTime = esp_timer_get_time();
  uint64_t lastBitmapTime = lastScanTime;
  uint64_t lastLogTime = lastScanTime;
  uint32_t timesExecuted = 0;

  for (;;)
  {
    uint64_t time = esp_timer_get_time();

    if (time - lastScanTime >= keyScanInterval - 1)
    {
      keyScanner.updateKeyState();
      timesExecuted++;
      lastScanTime = time;

      if (time - lastBitmapTime >= bitmapSendInterval - 1)
      {
        keyScanner.copyPublishedBitmap(localBitmap.data(), localBitmap.size());
        uint8_t bitmapSize = static_cast<uint8_t>(keyScanner.getBitMapSize());
        sendBitMapEvent(bitmapSize, localBitmap.data());
        lastBitmapTime = time;
        log.debug("Bitmap sent");
      }

      if (time - lastLogTime >= 5000000)
      {
        float avgUpdateInterval = (time - lastLogTime) / timesExecuted;
        float avgRefreshRateHz = timesExecuted / 5;
        log.debug("Average Keyscan update rate (Hz): %.2f, interval (uS): %.1f", avgRefreshRateHz, avgUpdateInterval);
        lastLogTime = time;
        timesExecuted = 0;
      }
    }
    else
      vPortYield();
  }
}

// KeyScanner helper functions

void KeyScannerTask::start(TaskParameters params)
{
  log.setMode(Logger::LogMode::Global);

  if (keyScannerTaskHandle != nullptr)
  {
    log.warn("KeyScannerTask already running");
    return;
  }

  log.info("Starting KeyScannerTask with stack size %u, priority %d, core affinity %d",
           params.stackSize, params.priority, params.coreAffinity);

  BaseType_t result = xTaskCreatePinnedToCore(
      taskEntry, "KeyScannerTask", params.stackSize, this,
      params.priority, &keyScannerTaskHandle, params.coreAffinity);
  if (result != pdPASS)
  {
    log.error("Failed to create KeyScannerTask");
    keyScannerTaskHandle = nullptr;
  }
}

void KeyScannerTask::stop()
{
  log.info("Stopping KeyScannerTask");
  if (keyScannerTaskHandle == nullptr)
  {
    log.info("Stop called but KeyScannerTask is not running");
    return;
  }
  vTaskDelete(keyScannerTaskHandle);
  keyScannerTaskHandle = nullptr;
}

void KeyScannerTask::restart(TaskParameters params)
{
  log.info("Restarting KeyScannerTask");
  if (keyScannerTaskHandle != nullptr)
    stop();
  start(params);
}