#include <shared/CommTypes.h>
#include <system/TaskManager.h>

void TaskManager::masterEspTask(void *arg) {
  MasterEspParameters *params = static_cast<MasterEspParameters *>(arg);

  QueueHandle_t EventBusQueueReference = params->EventBusQueue;
  IEspNow &espNow = *params->espNow;

  delete params;

  auto pairReceiveCallback = [&espNow](uint8_t *data, size_t length,
                                       uint8_t senderMac[6]) {
    espNow.sendData(static_cast<uint8_t>(PacketType::Pairing), data, length);
  };

  auto keyReceiveCallback = [EventBusQueueReference](uint8_t *data,
                                                     size_t length,
                                                     uint8_t senderMac[6]) {
    AirKeyEvent espKeyEvent;
    memcpy(&espKeyEvent, data, length);

    KeyEvent keyEvent;
    keyEvent.keyIndex = espKeyEvent.keyIndex;
    keyEvent.state = espKeyEvent.state;
    keyEvent.sourceMac = senderMac;
    xQueueSend(EventBusQueueReference, &keyEvent, pdMS_TO_TICKS(20));
  };

  auto bitmapReceiveCallback = [EventBusQueueReference](uint8_t *data,
                                                        size_t length,
                                                        uint8_t senderMac[6]) {
    AirBitmapEvent airBitmapEvent;
    memcpy(&airBitmapEvent, data, length);

    BitMapEvent bitmapEvent;
    bitmapEvent.bitMapData = airBitmapEvent.bitMapData;
    bitmapEvent.bitMapSize = airBitmapEvent.bitMapSize;
    bitmapEvent.sourceMac = senderMac;
    xQueueSend(EventBusQueueReference, &bitmapEvent, pdMS_TO_TICKS(20));
  };

  espNow.registerPacketTypeCallback(static_cast<uint8_t>(PacketType::KeyEvent),
                                    keyReceiveCallback);
  espNow.registerPacketTypeCallback(static_cast<uint8_t>(PacketType::KeyBitmap),
                                    bitmapReceiveCallback);

  TickType_t previousWakeTime = xTaskGetTickCount();

  for (;;) {
  }
}