#include <shared/CommTypes.h>
#include <system/TaskManager.h>

void TaskManager::masterEspTask(void *arg)
{
  MasterEspParameters *params = static_cast<MasterEspParameters *>(arg);

  QueueHandle_t EventBusQueueReference = params->EventBusQueue;
  IEspNow &espNow = *params->espNow;

  delete params;

  auto pairReceiveCallback = [&espNow](const uint8_t *data, size_t length, const uint8_t senderMac[6])
  {
    bool sendSuccess = espNow.sendData(static_cast<uint8_t>(PacketType::Pairing), data, length, senderMac);
    printf("Received Pairing request %d, sent reply: %s\n", sendSuccess ? "success" : "failure");
  };

  auto keyReceiveCallback = [EventBusQueueReference](const uint8_t *data, size_t length, const uint8_t senderMac[6])
  {
    AirKeyEvent espKeyEvent;
    memcpy(&espKeyEvent, data, length);

    KeyEvent keyEvent;
    keyEvent.keyIndex = espKeyEvent.keyIndex;
    keyEvent.state = espKeyEvent.state;
    keyEvent.sourceMac = senderMac;
    printf("Sending key event to event bus\n");
    xQueueSend(EventBusQueueReference, &keyEvent, pdMS_TO_TICKS(20));
  };

  auto bitmapReceiveCallback = [EventBusQueueReference](const uint8_t *data, size_t length, const uint8_t senderMac[6])
  {
    AirBitmapEvent airBitmapEvent;
    memcpy(&airBitmapEvent, data, length);

    BitMapEvent bitmapEvent;
    bitmapEvent.bitMapData = airBitmapEvent.bitMapData;
    bitmapEvent.bitMapSize = airBitmapEvent.bitMapSize;
    bitmapEvent.sourceMac = senderMac;
    printf("Sending bitmap event to event bus\n");
    xQueueSend(EventBusQueueReference, &bitmapEvent, pdMS_TO_TICKS(20));
  };

  espNow.registerPacketTypeCallback(static_cast<uint8_t>(PacketType::KeyEvent), keyReceiveCallback);
  espNow.registerPacketTypeCallback(static_cast<uint8_t>(PacketType::KeyBitmap), bitmapReceiveCallback);
  espNow.registerPacketTypeCallback(static_cast<uint8_t>(PacketType::Pairing), pairReceiveCallback);

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

  MasterEspParameters *params = new MasterEspParameters();
  params->EventBusQueue = eventBusQueue;
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
  }
}

void TaskManager::stopMasterEspTask()
{
  if (masterEspHandle == nullptr)
    return;
  vTaskDelete(masterEspHandle);
  masterEspHandle = nullptr;
}

void TaskManager::restartMasterEspTask(IEspNow &espNow)
{
  if (masterEspHandle != nullptr)
    stopMasterEspTask();
  startMasterEspTask(espNow);
}