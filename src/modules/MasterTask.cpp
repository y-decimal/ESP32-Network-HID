#include <modules/MasterTask.h>

// Initialize static member variable
MasterTask *MasterTask::instance = nullptr;

MasterTask::MasterTask(ITransport &transport) : transportRef(&transport)
{
  instance = this;
}

MasterTask::~MasterTask()
{
  if (internalLog)
    delete internalLog;
  if (protocol)
    delete protocol;
  internalLog = nullptr;
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
  internalLog = new Logger(MASTERTASK_NAMESPACE);

  internalLog->info("Starting MasterTask with stack size %u, priority %d, core affinity %d",
                    params.stackSize, params.priority, params.coreAffinity);
  if (masterTaskHandle != nullptr)
  {
    internalLog->warn("MasterTask is already running!");
    return;
  }

  BaseType_t result = xTaskCreatePinnedToCore(taskEntry, "MasterTask",
                                              params.stackSize, this,
                                              params.priority, &masterTaskHandle,
                                              params.coreAffinity);

  if (result != pdPASS)
  {
    masterTaskHandle = nullptr;
    internalLog->error("Failed to create MasterTask!");
  }
}

void MasterTask::stop()
{
  internalLog->info("Stopping MasterTask");
  if (masterTaskHandle == nullptr)
  {
    internalLog->warn("Stop called but MasterTask is not running");
    return;
  }

  internalLog->info("Stopping MasterTask");
  if (masterTaskHandle == nullptr)
  {
    internalLog->warn("Stop called but MasterTask is not running");
    return;
  }

  vTaskDelete(masterTaskHandle);
  masterTaskHandle = nullptr;

  if (internalLog)
    delete internalLog;
  if (protocol)
    delete protocol;
  internalLog = nullptr;
  protocol = nullptr;
}

void MasterTask::restart(TaskParameters params)
{
  internalLog->info("Restarting MasterTask");
  if (masterTaskHandle != nullptr)
    stop();
  start(params);
}

void MasterTask::pairReceiveCallback(const uint8_t *data, uint8_t sourceId)
{
  MasterTask::instance->internalLog->info("Received pairing request from device ID %u", sourceId);
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
