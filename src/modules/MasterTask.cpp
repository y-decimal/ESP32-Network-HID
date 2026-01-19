#include <modules/MasterTask.h>

// Initialize static member variable
MasterTask *MasterTask::instance = nullptr;

MasterTask::MasterTask(ITransport &transport) : transportRef(&transport)
{
  instance = this;
}

MasterTask::~MasterTask()
{
  if (logger)
    delete logger;
  if (protocol)
    delete protocol;
  logger = nullptr;
  protocol = nullptr;
  instance = nullptr;
}

void MasterTask::taskEntry(void *arg)
{
  MasterTask *task = static_cast<MasterTask *>(arg);

  task->protocol->onKeyEvent(keyReceiveCallback);
  task->protocol->onBitmapEvent(bitmapReceiveCallback);
  task->protocol->onPairingRequest(pairReceiveCallback);

  for (;;)
  {
    // Todo: Implement config updates here
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void MasterTask::start(TaskParameters params)
{
  protocol = new TransportProtocol(*transportRef);
  logger = new Logger(MASTERTASK_NAMESPACE);

  logger->info("Starting MasterTask with stack size %u, priority %d, core affinity %d",
               params.stackSize, params.priority, params.coreAffinity);
  if (masterTaskHandle != nullptr)
  {
    logger->warn("MasterTask is already running!");
    return;
  }

  BaseType_t result = xTaskCreatePinnedToCore(taskEntry, "MasterTask",
                                              params.stackSize, this,
                                              params.priority, &masterTaskHandle,
                                              params.coreAffinity);

  if (result != pdPASS)
  {
    masterTaskHandle = nullptr;
    logger->error("Failed to create MasterTask!");
  }
}

void MasterTask::stop()
{
  logger->info("Stopping MasterTask");
  if (masterTaskHandle == nullptr)
  {
    logger->warn("Stop called but MasterTask is not running");
    return;
  }

  logger->info("Stopping MasterTask");
  if (masterTaskHandle == nullptr)
  {
    logger->warn("Stop called but MasterTask is not running");
    return;
  }

  vTaskDelete(masterTaskHandle);
  masterTaskHandle = nullptr;

  if (logger)
    delete logger;
  if (protocol)
    delete protocol;
  logger = nullptr;
  protocol = nullptr;
}

void MasterTask::restart(TaskParameters params)
{
  logger->info("Restarting MasterTask");
  if (masterTaskHandle != nullptr)
    stop();
  start(params);
}

void MasterTask::pairReceiveCallback(const uint8_t *data, uint8_t sourceId)
{
  MasterTask::instance->logger->info("Received pairing request from device ID %u", sourceId);
};

void MasterTask::keyReceiveCallback(const RawKeyEvent &keyEvent, uint8_t senderId)
{
  Event event = {};
  event.type = EventType::IdKey;
  event.idKeyEvt.raw = keyEvent;
  event.idKeyEvt.sourceId = senderId;
  event.cleanup = cleanupIdentifiableKeyEvent;
  EventRegistry::pushEvent(event);
};

void MasterTask::bitmapReceiveCallback(const RawBitmapEvent &bitmapEvent, uint8_t senderId)
{
  Event event = {};
  event.type = EventType::IdBitmap;
  event.idBitmapEvt.raw = bitmapEvent;
  event.idBitmapEvt.sourceId = senderId;
  event.cleanup = cleanupIdentifiableBitmapEvent;
  EventRegistry::pushEvent(event);
};
