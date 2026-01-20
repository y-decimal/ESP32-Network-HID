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
}

void KeyScannerTask::sendBitMapEvent(uint8_t bitMapSize, uint8_t *bitMap)
{
  RawBitmapEvent rBitmapEvent{};
  rBitmapEvent.bitMapSize = bitMapSize;
  rBitmapEvent.bitMapData = static_cast<uint8_t *>(malloc(bitMapSize));
  memcpy(rBitmapEvent.bitMapData, bitMap, bitMapSize);

  Event event{};
  event.type = EventType::RawBitmap;
  event.rawBitmapEvt = rBitmapEvent;
  event.cleanup = cleanupRawBitmapEvent;

  if (!EventRegistry::pushEvent(event))
  {
    log.error("Failed to push bitmap event to EventRegistry");
    event.cleanup(&event);
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

  keyScanner.registerOnKeyChangeCallback(keyEventCallback);

  TickType_t previousWakeTime = xTaskGetTickCount();
  TickType_t refreshRateToTicks =
      pdMS_TO_TICKS((1000 / localConfig.getRefreshRate()));

  // Calculate bitmap send interval in loops based on frequency
  uint16_t bitMapLoopInterval = localConfig.getRefreshRate() / localConfig.getBitMapSendInterval();
  if (bitMapLoopInterval == 0)
    bitMapLoopInterval = 1; // Minimum 1 loop if freq > refresh rate

  uint16_t loopsSinceLastBitMap = 0;
  std::vector<uint8_t> localBitmap;
  localBitmap.assign(localConfig.getBitmapSize(), 0);

  while (true)
  {
    loopsSinceLastBitMap++;
    keyScanner.updateKeyState();
    if (loopsSinceLastBitMap >= bitMapLoopInterval)
    {
      keyScanner.copyPublishedBitmap(localBitmap.data(), localBitmap.size());
      uint8_t bitMapSize = static_cast<uint8_t>(keyScanner.getBitMapSize());
      sendBitMapEvent(bitMapSize, localBitmap.data());
      loopsSinceLastBitMap = 0;
    }
    xTaskDelayUntil(&previousWakeTime, refreshRateToTicks);
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