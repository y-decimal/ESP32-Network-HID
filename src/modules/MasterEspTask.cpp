#include <system/TaskManager.h>
#include <submodules/EspNowProtocol.h>

static EspNowProtocol *protocol = nullptr;
static QueueHandle_t eventBusQueueReference = nullptr;

void pairReceiveCallback(const uint8_t *data, uint8_t sourceId);
void keyReceiveCallback(const RawKeyEvent &keyEvent, uint8_t senderId);
void bitmapReceiveCallback(const RawBitmapEvent &bitmapEvent, uint8_t senderId);

void TaskManager::masterEspTask(void *arg)
{
  MasterSlaveParameters *params = static_cast<MasterSlaveParameters *>(arg);

  eventBusQueueReference = params->eventBusQueue;
  protocol = new EspNowProtocol(*params->espNow);

  delete params;

  protocol->onKeyEvent(keyReceiveCallback);
  protocol->onBitmapEvent(bitmapReceiveCallback);
  protocol->onPairingRequest(pairReceiveCallback);

  TickType_t previousWakeTime = xTaskGetTickCount();

  for (;;)
  {
    // Todo: Implement config updates here
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void TaskManager::startMasterEspTask(IEspNow &espNow)
{
  if (masterEspHandle != nullptr)
    return;

  MasterSlaveParameters *params = new MasterSlaveParameters();
  params->eventBusQueue = eventBusQueue;
  params->espNow = &espNow;

  BaseType_t result = xTaskCreatePinnedToCore(
      masterEspTask,
      "MasterEspTask",
      STACK_MASTERESP,
      params,
      PRIORITY_MASTERESP,
      &masterEspHandle,
      CORE_MASTERESP);

  if (result != pdPASS)
  {
    masterEspHandle = nullptr;
    delete params;
    delete protocol;
    delete eventBusQueueReference;
  }
}

void TaskManager::stopMasterEspTask()
{
  if (masterEspHandle == nullptr)
    return;

  delete protocol;
  delete eventBusQueueReference;

  vTaskDelete(masterEspHandle);
  masterEspHandle = nullptr;
}

void TaskManager::restartMasterEspTask(IEspNow &espNow)
{
  if (masterEspHandle != nullptr)
    stopMasterEspTask();
  startMasterEspTask(espNow);
}

void pairReceiveCallback(const uint8_t *data, uint8_t sourceId)
{
  uint8_t senderMac[6] = {};
  protocol->getMacById(sourceId, senderMac);
  printf("Received Pairing request from %02x:%02x:%02x:%02x:%02x:%02x",
         senderMac[0], senderMac[1], senderMac[2], senderMac[3], senderMac[4], senderMac[5]);
};

void keyReceiveCallback(const RawKeyEvent &keyEvent, uint8_t senderId)
{
  Event event = {};
  event.type = EventType::IdKey;
  event.idKeyEvt.raw = keyEvent;
  event.idKeyEvt.sourceId = senderId;
  event.cleanup = cleanupIdentifiableKeyEvent;
  xQueueSend(eventBusQueueReference, &event, pdMS_TO_TICKS(20));
};

void bitmapReceiveCallback(const RawBitmapEvent &bitmapEvent, uint8_t senderId)
{
  Event event = {};
  event.type = EventType::IdBitmap;
  event.idBitmapEvt.raw = bitmapEvent;
  event.idBitmapEvt.sourceID = senderId;
  event.cleanup = cleanupIdentifiableBitmapEvent;
  xQueueSend(eventBusQueueReference, &event, pdMS_TO_TICKS(20));
};
