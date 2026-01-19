#include <modules/SlaveTask.h>

// Initialize static member variable
SlaveTask *SlaveTask::instance = nullptr;

SlaveTask::SlaveTask(ITransport &transport) : transportRef(&transport)
{
  instance = this;
  if (localQueue != nullptr)
    return;

  localQueue = xQueueCreate(16, sizeof(Event));
}

SlaveTask::~SlaveTask()
{
  internalLog->info("Destroying SlaveTask");
  instance = nullptr;
  if (localQueue != nullptr)
  {
    vQueueDelete(localQueue);
    localQueue = nullptr;
  }
  if (internalLog)
    delete internalLog;
  if (protocol)
    delete protocol;
  internalLog = nullptr;
  protocol = nullptr;
}

void SlaveTask::taskEntry(void *arg)
{
  SlaveTask *task = static_cast<SlaveTask *>(arg);

  EventRegistry::registerHandler(EventType::RawKey, eventBusCallback);
  EventRegistry::registerHandler(EventType::RawBitmap, eventBusCallback);

  task->protocol->onPairingConfirmation(pairConfirmCallback);
  task->protocol->onConfigReceived(configReceiveCallback);

  TickType_t previousWakeTime = xTaskGetTickCount();

  for (;;)
  {
    // If not connected, attempt to pair every X seconds
    if (!task->connected)
    {
      task->protocol->sendPairingRequest();
      xTaskDelayUntil(&previousWakeTime, pdMS_TO_TICKS(3500));
      continue;
    }

    // If connected, process key events from the queue
    // Wait for key events with a timeout of 1.5 seconds to allow periodic
    // connection checks and potential reconnections
    Event event;
    if (xQueueReceive(task->localQueue, &event, pdMS_TO_TICKS(1500)))
    {
      // Process KeyEvent
      if (event.type == EventType::RawKey)
      {
        task->protocol->sendKeyEvent(event.rawKeyEvt);
      }

      // Process BitMapEvent
      if (event.type == EventType::RawBitmap)
      {
        task->protocol->sendBitmapEvent(event.rawBitmapEvt);
      }

      // Clean up event resources
      if (event.cleanup)
        event.cleanup(&event);
    }
  }
}

void SlaveTask::start(TaskParameters params)
{
  internalLog = new Logger(SLAVETASK_NAMESPACE);
  protocol = new TransportProtocol(*transportRef);

  internalLog->info("Starting SlaveTask with stack size %u, priority %d, core affinity %d",
                    params.stackSize, params.priority, params.coreAffinity);

  if (slaveTaskHandle != nullptr)
  {
    internalLog->warn("SlaveTask is already running!");
    return;
  }

  BaseType_t result = xTaskCreatePinnedToCore(
      taskEntry, SLAVETASK_NAMESPACE, params.stackSize, this,
      params.priority, &slaveTaskHandle, params.coreAffinity);

  if (result != pdPASS)
  {
    slaveTaskHandle = nullptr;
    internalLog->error("Failed to create SlaveTask!");
  }
}

void SlaveTask::stop()
{
  internalLog->info("Stopping SlaveTask");
  if (slaveTaskHandle == nullptr)
  {
    internalLog->warn("Stop called but SlaveTask is not running");
    return;
  }

  if (localQueue != nullptr)
  {
    vQueueDelete(localQueue);
    localQueue = nullptr;
  }
  vTaskDelete(slaveTaskHandle);
  slaveTaskHandle = nullptr;

  if (internalLog)
    delete internalLog;
  if (protocol)
    delete protocol;
  internalLog = nullptr;
  protocol = nullptr;
}

void SlaveTask::restart(TaskParameters params)
{
  internalLog->info("Restarting SlaveTask");
  if (slaveTaskHandle != nullptr)
    stop();
  start(params);
}

void SlaveTask::eventBusCallback(const Event &evt)
{
  if (SlaveTask::instance == nullptr)
  {
    SlaveTask::instance->internalLog->error("SlaveTask instance is null in eventBusCallback");
    return;
  }

  if (SlaveTask::instance->localQueue != nullptr)
  {
    xQueueSend(SlaveTask::instance->localQueue, &evt, pdMS_TO_TICKS(10));
  }
}

void SlaveTask::pairConfirmCallback(const uint8_t *data, const uint8_t sourceId)
{
  if (SlaveTask::instance == nullptr)
  {
    SlaveTask::instance->internalLog->error("SlaveTask instance is null in pairConfirmCallback");
    return;
  }
  SlaveTask::instance->connected = true;
  uint8_t masterMac[6] = {};
  SlaveTask::instance->protocol->getMacById(sourceId, masterMac);
  SlaveTask::instance->internalLog->info("Received master MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
                                         masterMac[0], masterMac[1], masterMac[2],
                                         masterMac[3], masterMac[4], masterMac[5]);
}

void SlaveTask::configReceiveCallback(const ConfigManager &config, uint8_t senderId)
{
  if (SlaveTask::instance == nullptr)
  {
    SlaveTask::instance->internalLog->error("SlaveTask instance is null in configReceiveCallback");
    return;
  }
  SlaveTask::instance->internalLog->info("Received configuration update from master ID %u", senderId);
  return; // Todo: implement config handling
}