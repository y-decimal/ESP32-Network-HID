#include <shared/CommTypes.h>
#include <system/TaskManager.h>

static QueueHandle_t keyEventQueueReference = nullptr;

void keyEventRouteCallback(const Event &event)
{
  xQueueSend(keyEventQueueReference, &event, pdMS_TO_TICKS(10));
  printf("Routed key event to key event queue\n");
}

void TaskManager::slaveEspTask(void *arg)
{
  SlaveEspParameters *params = static_cast<SlaveEspParameters *>(arg);

  keyEventQueueReference = params->keyEventQueue;
  IEspNow &espNow = *params->espNow;

  EventRegistry::registerHandler(EventType::Key, keyEventRouteCallback);
  EventRegistry::registerHandler(EventType::BitMap, keyEventRouteCallback);

  delete params;

  volatile bool connected = false;
  uint8_t masterMac[6] = {0};
  uint8_t broadcastMac[6] = {255, 255, 255, 255, 255, 255};

  auto pairReceiveCallback = [&connected, &masterMac, &broadcastMac](const uint8_t *data, size_t length, const uint8_t *senderMac)
  {
    if (memcmp(senderMac, broadcastMac, 6) == 0)
    {
      printf("Received broadcast MAC, ignoring\n");
      return; // Ignore broadcasts
    }
    memcpy(masterMac, senderMac, 6);
    connected = true;
    printf("Received master MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
           masterMac[0], masterMac[1], masterMac[2],
           masterMac[3], masterMac[4], masterMac[5]);
  };

  auto configReceiveCallback = [](const uint8_t *data, size_t length, const uint8_t *senderMac)
  {
    // Todo: Handle config update packet
  };

  bool successPairing = espNow.registerPacketTypeCallback(static_cast<uint8_t>(PacketType::PairingConfirmation), pairReceiveCallback);

  bool successConfig = espNow.registerPacketTypeCallback(static_cast<uint8_t>(PacketType::Config), configReceiveCallback);

  printf("SlaveEspTask: Registered pairing callback: %s\n", successPairing ? "success" : "failure");
  printf("SlaveEspTask: Registered config callback: %s\n", successConfig ? "success" : "failure");

  TickType_t previousWakeTime = xTaskGetTickCount();
  uint8_t sequenceNumber = 0;

  for (;;)
  {
    // If not connected, attempt to pair every 1.5 seconds
    if (!connected)
    {
      bool sendSuccess = espNow.sendData(static_cast<uint8_t>(PacketType::PairingRequest),
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
          KeyEvent keyEvent = event.key;
          AirKeyEvent evt = {keyEvent.keyIndex, keyEvent.state};

          uint8_t data[sizeof(AirKeyEvent)] = {0};
          memcpy(data, &evt, sizeof(AirKeyEvent));

          espNow.sendData((uint8_t)PacketType::KeyEvent, data, sizeof(data), masterMac);
          printf("Sent key event to Mac %d %d %d %d %d %d\n", masterMac[0], masterMac[1], masterMac[2],
                 masterMac[3], masterMac[4], masterMac[5]);
        }

        // Process BitMapEvent
        if (event.type == EventType::BitMap)
        {
          uint8_t data[event.bitMap.bitMapSize + 1];
          data[0] = event.bitMap.bitMapSize;
          memcpy(data + 1, event.bitMap.bitMapData, event.bitMap.bitMapSize);

          espNow.sendData((uint8_t)PacketType::KeyBitmap, data, sizeof(data), masterMac);
          printf("Sent bitmap event to Mac %d %d %d %d %d %d\n", masterMac[0], masterMac[1], masterMac[2],
                 masterMac[3], masterMac[4], masterMac[5]);
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