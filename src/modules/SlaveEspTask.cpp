#include <submodules/TransportProtocol.h>
#include <system/TaskManager.h>

static TransportProtocol *protocol = nullptr;
QueueHandle_t localKeyQueue = nullptr;
static bool connected = false;

void eventBusCallback(const Event &evt);

void pairConfirmCallback(const uint8_t *data, const uint8_t sourceId);
void configReceiveCallback(const ConfigManager &config, uint8_t senderId);

void TaskManager::slaveEspTask(void *arg)
{
  MasterSlaveParameters *params = static_cast<MasterSlaveParameters *>(arg);
  protocol = new TransportProtocol(*params->espNow);

  delete params;

  EventRegistry::registerHandler(EventType::RawKey, eventBusCallback);
  EventRegistry::registerHandler(EventType::RawBitmap, eventBusCallback);

  protocol->onPairingConfirmation(pairConfirmCallback);
  protocol->onConfigReceived(configReceiveCallback);

  TickType_t previousWakeTime = xTaskGetTickCount();
  localKeyQueue = xQueueCreate(32, sizeof(Event));

  for (;;)
  {
    // If not connected, attempt to pair every X seconds
    if (!connected)
    {
      protocol->sendPairingRequest();
      xTaskDelayUntil(&previousWakeTime, pdMS_TO_TICKS(3500));
      continue;
    }

    // If connected, process key events from the queue
    // Wait for key events with a timeout of 1.5 seconds to allow periodic
    // connection checks and potential reconnections
    Event event;
    if (xQueueReceive(localKeyQueue, &event, pdMS_TO_TICKS(1500)))
    {
      // Process KeyEvent
      if (event.type == EventType::RawKey)
      {
        protocol->sendKeyEvent(event.rawKeyEvt);
      }

      // Process BitMapEvent
      if (event.type == EventType::RawBitmap)
      {
        protocol->sendBitmapEvent(event.rawBitmapEvt);
      }

      // Clean up event resources
      if (event.cleanup)
        event.cleanup(&event);
    }
  }
}

void TaskManager::startSlaveEspTask(ITransport &espNow)
{

  if (slaveEspHandle != nullptr)
    return;

  MasterSlaveParameters *params = new MasterSlaveParameters();
  params->eventBusQueue = eventBusQueue;
  params->espNow = &espNow;

  BaseType_t result = xTaskCreatePinnedToCore(
      slaveEspTask,
      "SlaveEspTask",
      STACK_SLAVEESP,
      params,
      PRIORITY_SLAVEESP,
      &slaveEspHandle,
      CORE_SLAVEESP);

  if (result != pdPASS)
  {
    slaveEspHandle = nullptr;
    delete params;
    protocol = nullptr;
  }
}

void TaskManager::stopSlaveEspTask()
{
  if (slaveEspHandle == nullptr)
    return;

  if (localKeyQueue != nullptr)
  {
    vQueueDelete(localKeyQueue);
    localKeyQueue = nullptr;
  }

  if (protocol != nullptr)
  {
    protocol->clearCallbacks();
    delete protocol;
    protocol = nullptr;
  }

  vTaskDelete(slaveEspHandle);
  slaveEspHandle = nullptr;
}

void TaskManager::restartSlaveEspTask(ITransport &espNow)
{
  if (slaveEspHandle != nullptr)
    stopSlaveEspTask();
  startSlaveEspTask(espNow);
}

void eventBusCallback(const Event &evt)
{
  if (localKeyQueue != nullptr)
  {
    xQueueSend(localKeyQueue, &evt, pdMS_TO_TICKS(10));
  }
}

void pairConfirmCallback(const uint8_t *data, const uint8_t sourceId)
{
  connected = true;
  uint8_t masterMac[6] = {};
  protocol->getMacById(sourceId, masterMac);
  printf("Received master MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
         masterMac[0], masterMac[1], masterMac[2],
         masterMac[3], masterMac[4], masterMac[5]);
}

void configReceiveCallback(const ConfigManager &config, uint8_t senderId)
{
  return; // Todo: implement config handling
}