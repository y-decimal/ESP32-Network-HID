#include <shared/CommTypes.h>
#include <system/TaskManager.h>

void TaskManager::slaveEspTask(void *arg)
{
  SlaveEspParameters *params = static_cast<SlaveEspParameters *>(arg);

  QueueHandle_t keyEventQueueReference = params->keyEventQueue;
  IEspNow &espNow = *params->espNow;

  delete params;

  bool connected = false;
  uint8_t masterMac[6] = {0};

  auto pairReceiveCallback = [&connected, &masterMac](const uint8_t *data, size_t length, const uint8_t senderMac[6])
  {
    memcpy(masterMac, senderMac, 6);
    connected = true;
  };

  auto configReceiveCallback = [](const uint8_t *data, size_t length, const uint8_t senderMac[6])
  {
    // Todo: Handle config update packet
  };

  espNow.registerPacketTypeCallback(static_cast<uint8_t>(PacketType::Pairing), pairReceiveCallback);

  espNow.registerPacketTypeCallback(static_cast<uint8_t>(PacketType::Config), configReceiveCallback);

  TickType_t previousWakeTime = xTaskGetTickCount();
  uint8_t sequenceNumber = 0;

  for (;;)
  {
    // If not connected, attempt to pair every 1.5 seconds
    if (!connected)
    {
      uint8_t broadcastMac[6] = {255, 255, 255, 255, 255};
      bool sendSuccess = espNow.sendData(static_cast<uint8_t>(PacketType::Pairing),
                                         &sequenceNumber,
                                         sizeof(sequenceNumber),
                                         broadcastMac);
      printf("Sent pairing request %d: %s\n", sequenceNumber, sendSuccess ? "success" : "fail");
      sequenceNumber++;
      if (sequenceNumber == 255)
        sequenceNumber = 0;
      xTaskDelayUntil(&previousWakeTime, pdMS_TO_TICKS(3500));
    }
    // If connected, process key events from the queue
    else
    {

      Event event;

      // Wait for key events with a timeout of 1.5 seconds to allow periodic
      // connection checks and potential reconnections
      if (xQueueReceive(keyEventQueueReference, &event, pdMS_TO_TICKS(1500)))
      {

        // Process KeyEvent
        if (event.type == EventType::Key)
        {
          AirKeyEvent evt = {event.key.keyIndex, event.key.state};

          uint8_t data[sizeof(AirKeyEvent)] = {};
          memcpy(data, &evt, sizeof(AirKeyEvent));

          espNow.sendData((uint8_t)PacketType::KeyEvent, data, sizeof(data), masterMac);
        }

        // Process BitMapEvent
        if (event.type == EventType::BitMap)
        {
          uint8_t data[event.bitMap.bitMapSize + 1];
          data[0] = event.bitMap.bitMapSize;
          memcpy(data + 1, event.bitMap.bitMapData, event.bitMap.bitMapSize);

          espNow.sendData((uint8_t)PacketType::KeyBitmap, data, sizeof(data), masterMac);
        }

        // Clean up event resources
        if (event.cleanup)
          event.cleanup(&event);
      }
    }
    vPortYield();
  }
}

void TaskManager::startSlaveEspTask(IEspNow &espNow)
{

  if (slaveEspHandle != nullptr)
    return;

  SlaveEspParameters *params = new SlaveEspParameters();
  params->keyEventQueue = keyEventQueue;
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
  }
}

void TaskManager::stopSlaveEspTask()
{
  if (slaveEspHandle == nullptr)
    return;
  vTaskDelete(slaveEspHandle);
  slaveEspHandle = nullptr;
}

void TaskManager::restartSlaveEspTask(IEspNow &espNow)
{
  if (slaveEspHandle != nullptr)
    stopSlaveEspTask();
  startSlaveEspTask(espNow);
}