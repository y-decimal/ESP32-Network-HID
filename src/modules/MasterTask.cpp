#include <modules/MasterTask.h>
#include <submodules/Logger.h>

static Logger log(MASTERTASK_NAMESPACE);

// Initialize static member variable
MasterTask *MasterTask::instance = nullptr;
HidMapper MasterTask::hidMapper;
std::vector<uint8_t> MasterTask::oldBitmap = {0};

MasterTask::MasterTask(ITransport &transport) : transportRef(&transport)
{
  if (instance)
  {
    log.warn("MasterTask instance already exists, replacing");
    delete instance;
  }
  instance = this;

  oldBitmap.resize(hidMapper.getBitmapSize());
}

MasterTask::~MasterTask()
{
  stop();
  instance = nullptr;
}

void MasterTask::taskEntry(void *arg)
{
  MasterTask *task = static_cast<MasterTask *>(arg);

  task->protocol->onKeyEvent(keyReceiveCallback);
  task->protocol->onBitmapEvent(bitmapReceiveCallback);
  task->protocol->onPairingRequest(pairReceiveCallback);
  task->protocol->onConfigReceived(configReceiveCallback);
  log.debug("Registered TransportProtocol callbacks");

  for (;;)
  {
    // Todo: Implement config updates here
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void MasterTask::start(TaskParameters params)
{
  log.setMode(Logger::LogMode::Global);

  log.info("Starting MasterTask with stack size %u, priority %d, core affinity %d",
           params.stackSize, params.priority, params.coreAffinity);

  if (masterTaskHandle != nullptr)
  {
    log.warn("MasterTask is already running!");
    return;
  }

  protocol = new TransportProtocol(*transportRef);

  BaseType_t result = xTaskCreatePinnedToCore(taskEntry, "MasterTask",
                                              params.stackSize, this,
                                              params.priority, &masterTaskHandle,
                                              params.coreAffinity);

  if (result != pdPASS)
  {
    masterTaskHandle = nullptr;
    log.error("Failed to create MasterTask!");
    delete (protocol);
    protocol = nullptr;
  }
}

void MasterTask::stop()
{
  log.info("Stopping MasterTask");
  if (masterTaskHandle == nullptr)
  {
    log.warn("Stop called but MasterTask is not running");
    return;
  }

  if (protocol)
    delete protocol;
  protocol = nullptr;

  vTaskDelete(masterTaskHandle);
  masterTaskHandle = nullptr;
}

void MasterTask::restart(TaskParameters params)
{
  log.info("Restarting MasterTask");
  if (masterTaskHandle != nullptr)
    stop();
  start(params);
}

void MasterTask::pairReceiveCallback(uint8_t sourceId)
{
  log.info("Received pairing request from device ID %u", sourceId);
  instance->protocol->requestConfig(sourceId);
};

void MasterTask::keyReceiveCallback(const RawKeyEvent &keyEvent, uint8_t senderId)
{
  if (instance->hidMapper.doesMapExist(senderId) == false)
  {
    instance->protocol->requestConfig(senderId);
    log.warn("No HID map for device ID %u, requested config", senderId);
    return;
  }

  instance->hidMapper.mapIndexToHidBitmap(keyEvent.keyIndex, keyEvent.state, senderId);
  log.debug("Pushed key event from device ID %u to HidMapper", senderId);

  std::vector<uint8_t> currentBitmap{0};
  currentBitmap.resize(hidMapper.getBitmapSize());
  hidMapper.copyBitmap(currentBitmap.data(), currentBitmap.size());
  if (memcmp(oldBitmap.data(), currentBitmap.data(), currentBitmap.size()) != 0)
  {
    // Push HID Event
    log.info("Hid Map changed, pushing HidEvent");
    HidBitmapEvent hidBitmapEvt{};
    hidBitmapEvt.bitmapSize = static_cast<uint8_t>(currentBitmap.size());
    hidBitmapEvt.bitMapData = static_cast<uint8_t *>(malloc(currentBitmap.size()));
    memcpy(hidBitmapEvt.bitMapData, currentBitmap.data(), currentBitmap.size());

    Event hidEvent{};
    hidEvent.type = EventType::HidBitmap;
    hidEvent.hidBitmapEvt = hidBitmapEvt;
    hidEvent.cleanup = cleanupHidBitmapEvent;

    if (!EventRegistry::pushEvent(hidEvent))
    {
      log.error("Failed to push HID bitmap event to EventRegistry");
      hidEvent.cleanup(&hidEvent);
    }
    else
    {
      log.info("Pushed HID bitmap event of size %d", hidBitmapEvt.bitmapSize);
    }
  }
  else
    log.info("No change to Hid Map");
  oldBitmap = currentBitmap;
};

void MasterTask::bitmapReceiveCallback(const RawBitmapEvent &bitmapEvent, uint8_t senderId)
{
  if (instance->hidMapper.doesMapExist(senderId) == false)
  {
    instance->protocol->requestConfig(senderId);
    log.warn("No HID map for device ID %u, requested config", senderId);
    return;
  }

  instance->hidMapper.mapBitmapToHidBitmap(bitmapEvent.bitMapData, bitmapEvent.bitmapSize, senderId);
  log.debug("Pushed bitmap event from device ID %u to HidMapper", senderId);

  std::vector<uint8_t> currentBitmap{0};
  currentBitmap.resize(hidMapper.getBitmapSize());
  hidMapper.copyBitmap(currentBitmap.data(), currentBitmap.size());
  if (memcmp(oldBitmap.data(), currentBitmap.data(), currentBitmap.size()) != 0)
  {
    // Push HID Event
    log.info("Hid Map changed, pushing HidEvent");
    HidBitmapEvent hidBitmapEvt{};
    hidBitmapEvt.bitmapSize = static_cast<uint8_t>(currentBitmap.size());
    hidBitmapEvt.bitMapData = static_cast<uint8_t *>(malloc(currentBitmap.size()));
    memcpy(hidBitmapEvt.bitMapData, currentBitmap.data(), currentBitmap.size());

    Event hidEvent{};
    hidEvent.type = EventType::HidBitmap;
    hidEvent.hidBitmapEvt = hidBitmapEvt;
    hidEvent.cleanup = cleanupHidBitmapEvent;

    if (!EventRegistry::pushEvent(hidEvent))
    {
      log.error("Failed to push HID bitmap event to EventRegistry");
      hidEvent.cleanup(&hidEvent);
    }
    else
    {
      log.info("Pushed HID bitmap event of size %d", hidBitmapEvt.bitmapSize);
    }
  }
  else
    log.debug("No change to Hid Map");

  oldBitmap = currentBitmap;
};

void MasterTask::configReceiveCallback(const ConfigManager &config, uint8_t senderId)
{
  std::vector<uint8_t> map = config.getConfig<KeyScannerConfig>().getLocalToHidMap();

  hidMapper.insertMap(map.data(), map.size(), senderId);

  log.info("Received Map from device %d", senderId);
}
