#ifndef TEST_FAKE_ESPNOW_H
#define TEST_FAKE_ESPNOW_H

#include <functional>
#include <interfaces/IEspNow.h>

class FakeEspNow : public IEspNow {
public:
  bool sendData(uint8_t packetType, uint8_t *data, size_t length) override {
    // do nothing
    return true;
  }
  bool registerPacketTypeCallback(uint8_t packetType,
                                  receiveCallback callback) override {
    callbacks[packetType] = callback;
    return true;
  }
  void simulateReceiveData(uint8_t packetType, uint8_t *data, size_t length,
                           uint8_t senderMac[6]) {
    if (callbacks[packetType]) {
      callbacks[packetType](data, length, senderMac);
    }
  }

private:
  receiveCallback callbacks[256];
};

#endif