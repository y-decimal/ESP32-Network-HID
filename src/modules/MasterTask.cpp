#include <modules/MasterTask.h>
#include <submodules/Logger.h>

static Logger log(MASTERTASK_NAMESPACE);

// Initialize static member variable
MasterTask *MasterTask::instance = nullptr;

MasterTask::MasterTask(ITransport &transport) : transportRef(&transport)
{
  if (instance)
  {
    log.warn("MasterTask instance already exists, replacing");
    delete instance;
  }
  instance = this;
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

void MasterTask::pairReceiveCallback(const uint8_t *data, uint8_t sourceId)
{
  log.info("Received pairing request from device ID %u", sourceId);
};

void MasterTask::keyReceiveCallback(const RawKeyEvent &keyEvent, uint8_t senderId)
{
  Event event = {};
  event.type = EventType::IdKey;
  event.idKeyEvt.raw = keyEvent;
  event.idKeyEvt.sourceId = senderId;
  event.cleanup = cleanupIdentifiableKeyEvent;
  EventRegistry::pushEvent(event);
  log.debug("Pushed key event from device ID %u to EventRegistry", senderId);
};

void MasterTask::bitmapReceiveCallback(const RawBitmapEvent &bitmapEvent, uint8_t senderId)
{
  Event event = {};
  event.type = EventType::IdBitmap;
  event.idBitmapEvt.raw = bitmapEvent;
  event.idBitmapEvt.sourceId = senderId;
  event.cleanup = cleanupIdentifiableBitmapEvent;
  EventRegistry::pushEvent(event);
  log.debug("Pushed bitmap event from device ID %u to EventRegistry", senderId);
};
