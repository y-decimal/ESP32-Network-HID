#include <modules/SlaveTask.h>
#include <submodules/Logger.h>

static Logger log(SLAVETASK_NAMESPACE);

// Initialize static member variable
SlaveTask *SlaveTask::instance = nullptr;

SlaveTask::SlaveTask(ITransport &transport) : transportRef(&transport)
{
  if (instance != nullptr)
  {
    log.warn("SlaveTask instance already exists, replacing");
    delete instance;
  }
  instance = this;
}

SlaveTask::~SlaveTask()
{
  stop();
  instance = nullptr;
}

void SlaveTask::taskEntry(void *arg)
{
  SlaveTask *task = static_cast<SlaveTask *>(arg);

  EventRegistry::registerHandler(EventType::RawKey, eventBusCallback);
  EventRegistry::registerHandler(EventType::RawBitmap, eventBusCallback);
  log.debug("Registered EventBus callbacks");

  task->protocol->onPairingConfirmation(pairConfirmCallback);
  task->protocol->onConfigReceived(configReceiveCallback);
  log.debug("Registered TransportProtocol callbacks");

  TickType_t previousWakeTime = xTaskGetTickCount();

  for (;;)
  {
    // If not connected, attempt to pair every X seconds
    if (!task->connected)
    {
      task->protocol->sendPairingRequest();
      log.debug("Sent pairing request to master");
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
        log.debug("Sent key event to master");
      }

      // Process BitMapEvent
      if (event.type == EventType::RawBitmap)
      {
        task->protocol->sendBitmapEvent(event.rawBitmapEvt);
        log.debug("Sent bitmap event to master");
      }

      // Clean up event resources
      if (event.cleanup)
      {
        event.cleanup(&event);
        log.debug("Cleaned up event resources");
      }
      else
      {
        log.debug("No cleanup function for event");
      }
    }
  }
}

void SlaveTask::start(TaskParameters params)
{
  log.setMode(Logger::LogMode::Global);

  localQueue = xQueueCreate(32, sizeof(Event));
  if (!localQueue)
  {
    log.error("Failed to create SlaveTask queue");
    return;
  }

  log.info("Starting SlaveTask with stack size %u, priority %d, core affinity %d",
           params.stackSize, params.priority, params.coreAffinity);

  if (slaveTaskHandle != nullptr)
  {
    log.warn("SlaveTask is already running!");
    return;
  }

  protocol = new TransportProtocol(*transportRef);

  BaseType_t result = xTaskCreatePinnedToCore(
      taskEntry, SLAVETASK_NAMESPACE, params.stackSize, this,
      params.priority, &slaveTaskHandle, params.coreAffinity);

  if (result != pdPASS)
  {
    slaveTaskHandle = nullptr;
    log.error("Failed to create SlaveTask!");
    delete (protocol);
    protocol = nullptr;
  }
}

void SlaveTask::stop()
{
  log.info("Stopping SlaveTask");
  if (slaveTaskHandle == nullptr)
  {
    log.warn("Stop called but SlaveTask is not running");
    return;
  }

  if (localQueue)
    vQueueDelete(localQueue);
  localQueue = nullptr;

  if (protocol)
    delete protocol;
  protocol = nullptr;

  vTaskDelete(slaveTaskHandle);
  slaveTaskHandle = nullptr;
}

void SlaveTask::restart(TaskParameters params)
{
  log.info("Restarting SlaveTask");
  if (slaveTaskHandle != nullptr)
    stop();
  start(params);
}

void SlaveTask::eventBusCallback(const Event &evt)
{
  if (SlaveTask::instance == nullptr)
  {
    log.error("SlaveTask instance is null in eventBusCallback");
    return;
  }

  if (SlaveTask::instance->localQueue != nullptr)
  {
    xQueueSend(SlaveTask::instance->localQueue, &evt, pdMS_TO_TICKS(10));
    log.debug("Event pushed to SlaveTask queue");
  }
}

void SlaveTask::pairConfirmCallback(uint8_t sourceId)
{
  if (SlaveTask::instance == nullptr)
  {
    log.error("SlaveTask instance is null in pairConfirmCallback");
    return;
  }
  SlaveTask::instance->connected = true;
  uint8_t masterMac[6] = {};
  SlaveTask::instance->protocol->getMacById(sourceId, masterMac);
  log.info("Received master MAC: %02x:%02x:%02x:%02x:%02x:%02x",
           masterMac[0], masterMac[1], masterMac[2],
           masterMac[3], masterMac[4], masterMac[5]);
}

void SlaveTask::configReceiveCallback(const ConfigManager &config, uint8_t senderId)
{
  if (SlaveTask::instance == nullptr)
  {
    log.error("SlaveTask instance is null in configReceiveCallback");
    return;
  }
  log.info("Received configuration update from master ID %u", senderId);
  return; // Todo: implement config handling
}