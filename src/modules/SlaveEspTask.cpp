#include <shared/CommTypes.h>
#include <system/TaskManager.h>

void TaskManager::slaveEspTask(void *arg) {
  EspParameters *params = static_cast<EspParameters *>(arg);

  QueueHandle_t keyEventQueueReference = params->keyEventHandle;
  IEspNow &espNow = *params->espNow;

  delete params;

  bool connected = false;

  auto pairReceiveCallback = [&connected](uint8_t *data, size_t length,
                                          uint8_t senderMac[6]) {
    connected = true;
    // Todo: Store senderMac for future communication, likely with config event
    // push to event bus
  };

  auto configReceiveCallback = [](uint8_t *data, size_t length,
                                  uint8_t senderMac[6]) {
    // Todo: Handle config update packet
  };

  espNow.registerPacketTypeCallback(static_cast<uint8_t>(PacketType::Pairing),
                                    pairReceiveCallback);

  espNow.registerPacketTypeCallback(static_cast<uint8_t>(PacketType::Config),
                                    configReceiveCallback);

  TickType_t previousWakeTime = xTaskGetTickCount();

  for (;;) {
    // If not connected, attempt to pair every 1.5 seconds
    if (!connected) {
      static uint8_t sequenceNumber =
          0; // Currently unused but we need to send something anyways
      espNow.sendData(static_cast<uint8_t>(PacketType::Pairing),
                      &sequenceNumber, sizeof(sequenceNumber));
      sequenceNumber++;
      xTaskDelayUntil(&previousWakeTime, pdMS_TO_TICKS(1500));

      // If connected, process key events from the queue
    } else {

      Event event;

      // Wait for key events with a timeout of 1.5 seconds to allow periodic
      // connection checks and potential reconnections
      if (xQueueReceive(keyEventQueueReference, &event, pdMS_TO_TICKS(1500))) {

        // Process KeyEvent
        if (event.type == EventType::Key) {
          EspKeyEvent evt = {event.key.keyIndex, event.key.state};

          uint8_t data[sizeof(EspKeyEvent)] = {};
          memcpy(data, &evt, sizeof(EspKeyEvent));

          espNow.sendData((uint8_t)PacketType::KeyEvent, data, sizeof(data));
        }

        // Process BitMapEvent
        if (event.type == EventType::BitMap) {
          uint8_t data[event.bitMap.bitMapSize + 1];
          data[0] = event.bitMap.bitMapSize;
          memcpy(data + 1, event.bitMap.bitMapData, event.bitMap.bitMapSize);

          espNow.sendData((uint8_t)PacketType::KeyBitmap, data, sizeof(data));
        }

        // Clean up event resources
        if (event.cleanup)
          event.cleanup(&event);
      }
    }
  }
}

void TaskManager::startSlaveEspTask(IEspNow &espNow) {

  if (slaveEspHandle != nullptr)
    return;

  EspParameters *params = new EspParameters();
  params->keyEventHandle = keyEventQueue;
  params->espNow = &espNow;

  BaseType_t result = xTaskCreatePinnedToCore(
      slaveEspTask, "SlaveEspTask", STACK_SLAVEESP, params, PRIORITY_SLAVEESP,
      &slaveEspHandle, CORE_SLAVEESP);

  if (result != pdPASS) {
    slaveEspHandle = nullptr;
    delete params;
  }
}

void TaskManager::stopSlaveEspTask() {
  if (slaveEspHandle == nullptr)
    return;
  vTaskDelete(slaveEspHandle);
  slaveEspHandle = nullptr;
}

void TaskManager::restartSlaveEspTask(IEspNow &espNow) {
  if (slaveEspHandle != nullptr)
    stopSlaveEspTask();
  startSlaveEspTask(espNow);
}