#include <shared/CommTypes.h>
#include <system/TaskManager.h>

void TaskManager::masterEspTask(void *arg) {
  EspParameters *params = static_cast<EspParameters *>(arg);

  QueueHandle_t keyEventQueueReference = params->keyEventHandle;
  IEspNow &espNow = *params->espNow;

  delete params;

  auto pairReceiveCallback = [&espNow](uint8_t *data, size_t length,
                                       uint8_t senderMac[6]) {
    espNow.sendData(static_cast<uint8_t>(PacketType::Pairing), data, length);
  };

  auto keyReceiveCallback = [keyEventQueueReference](uint8_t *data,
                                                     size_t length,
                                                     uint8_t senderMac[6]) {
    AirKeyEvent espKeyEvent;
    memcpy(&espKeyEvent, data, length);

    KeyEvent keyEvent;
    keyEvent.keyIndex = espKeyEvent.keyIndex;
    keyEvent.state = espKeyEvent.state;
    keyEvent.sourceMac = senderMac;
    xQueueSend(keyEventQueueReference, &keyEvent, pdMS_TO_TICKS(20));
  };

  auto bitmapReceiveCallback = [keyEventQueueReference](uint8_t *data,
                                                        size_t length,
                                                        uint8_t senderMac[6]) {
    AirBitmapEvent airBitmapEvent;
    memcpy(&airBitmapEvent, data, length);

    BitMapEvent bitmapEvent;
    bitmapEvent.bitMapData = airBitmapEvent.bitMapData;
    bitmapEvent.bitMapSize = airBitmapEvent.bitMapSize;
    bitmapEvent.sourceMac = senderMac;
    xQueueSend(keyEventQueueReference, &bitmapEvent, pdMS_TO_TICKS(20));
  };
}