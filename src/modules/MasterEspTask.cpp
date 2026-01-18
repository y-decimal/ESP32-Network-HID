#include <shared/CommTypes.h>
#include <system/TaskManager.h>

void TaskManager::masterEspTask(void *arg)
{
  MasterEspParameters *params = static_cast<MasterEspParameters *>(arg);

  QueueHandle_t eventBusQueueReference = params->eventBusQueue;
  IEspNow &espNow = *params->espNow;

  delete params;

  auto pairReceiveCallback = [&espNow](const uint8_t *data, size_t length, const uint8_t *senderMac)
  {
    bool sendSuccess = espNow.sendData(static_cast<uint8_t>(PacketType::PairingConfirmation), data, length, senderMac);
    uint8_t receivedSeqNum = 0;
    memcpy(&receivedSeqNum, data, length);
    printf("Received Pairing request %d from %02x:%02x:%02x:%02x:%02x:%02x, sent reply: %s\n",
           receivedSeqNum,
           senderMac[0], senderMac[1], senderMac[2], senderMac[3], senderMac[4], senderMac[5],
           sendSuccess ? "success" : "failure");
  };

  auto keyReceiveCallback = [eventBusQueueReference](const uint8_t *data, size_t length, const uint8_t *senderMac)
  {
    AirKeyEvent espKeyEvent = {};
    memcpy(&espKeyEvent, data, length);

    KeyEvent keyEvent;
    keyEvent.keyIndex = espKeyEvent.keyIndex;
    keyEvent.state = espKeyEvent.state;
    keyEvent.sourceMac = senderMac;

    Event event = {};
    event.type = EventType::Key;
    event.key = keyEvent;
    event.cleanup = cleanupKeyEvent;

    printf("Sending key event to event bus: key %d %s\n", keyEvent.keyIndex, keyEvent.state ? "pressed" : "released");
    xQueueSend(eventBusQueueReference, &event, pdMS_TO_TICKS(20));
  };

  auto bitmapReceiveCallback = [eventBusQueueReference](const uint8_t *data, size_t length, const uint8_t *senderMac)
  {
    if (length < 1)
    {
      printf("Invalid bitmap packet: too short\n");
      return;
    }

    uint8_t bitMapSize = data[0];

    // Allocate memory for the bitmap data
    uint8_t *bitMapData = static_cast<uint8_t *>(malloc(bitMapSize));
    if (!bitMapData)
    {
      printf("Failed to allocate memory for bitmap\n");
      return;
    }

    // Copy the bitmap data (skip first byte which is the size)
    memcpy(bitMapData, data + 1, bitMapSize);

    BitMapEvent bitmapEvent;
    bitmapEvent.bitMapSize = bitMapSize;
    bitmapEvent.bitMapData = bitMapData;
    bitmapEvent.sourceMac = senderMac;

    Event event = {};
    event.type = EventType::BitMap;
    event.bitMap = bitmapEvent;
    event.cleanup = cleanupBitmapEvent;

    printf("Sending bitmap event to event bus (size: %d)\n", bitMapSize);
    xQueueSend(eventBusQueueReference, &event, pdMS_TO_TICKS(20));
  };

  espNow.registerPacketTypeCallback(static_cast<uint8_t>(PacketType::KeyEvent), keyReceiveCallback);
  espNow.registerPacketTypeCallback(static_cast<uint8_t>(PacketType::KeyBitmap), bitmapReceiveCallback);
  espNow.registerPacketTypeCallback(static_cast<uint8_t>(PacketType::PairingRequest), pairReceiveCallback);

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