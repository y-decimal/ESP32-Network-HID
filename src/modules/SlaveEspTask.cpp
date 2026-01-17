#include <system/TaskManager.h>

void TaskManager::slaveEspTask(void *arg) {
  slaveEspParameters *params = static_cast<slaveEspParameters *>(arg);

  QueueHandle_t keyEventQueueReference = params->keyEventHandle;
  IEspNow &espNow = *params->espNow;

  bool connected = false;
  enum class PacketType : uint8_t { KeyEvent, BitMapEvent, PairRequest, COUNT };

  struct espKeyEvent {
    uint16_t keyIndex;
    bool state;
  };

  TickType_t previousWakeTime = xTaskGetTickCount();

  for (;;) {
    // If not connected, attempt to pair every 1.5 seconds
    if (!connected) {
      static uint8_t sequenceNumber =
          0; // Currently unused but we need to send something anyways
      espNow.sendData(static_cast<uint8_t>(PacketType::PairRequest),
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
          espKeyEvent evt = {event.key.keyIndex, event.key.state};

          uint8_t data[sizeof(espKeyEvent)] = {};
          memcpy(data, &evt, sizeof(espKeyEvent));

          espNow.sendData((uint8_t)PacketType::KeyEvent, data, sizeof(data));
        }

        // Process BitMapEvent
        if (event.type == EventType::BitMap) {
          uint8_t data[event.bitMap.bitMapSize + 1];
          data[0] = event.bitMap.bitMapSize;
          memcpy(data + 1, event.bitMap.bitMapData, event.bitMap.bitMapSize);

          espNow.sendData((uint8_t)PacketType::BitMapEvent, data, sizeof(data));
        }

        // Clean up event resources
        if (event.cleanup)
          event.cleanup(&event);
      }
    }
  }
}