#ifndef TEST_FAKE_ESPNOW_H
#define TEST_FAKE_ESPNOW_H

#include <functional>
#include <interfaces/ITransport.h>

class FakeEspNow : public ITransport
{
public:
  bool sendData(uint8_t packetType, const uint8_t *data, size_t length, const uint8_t *targetMac) override
  {
    // do nothing
    return true;
  }
  bool registerPacketTypeCallback(uint8_t packetType,
                                  receiveCallback callback) override
  {
    callbacks[packetType] = callback;
    return true;
  }
  void simulateReceiveData(uint8_t packetType, const uint8_t *data, size_t length, const uint8_t *senderMac)
  {
    if (callbacks[packetType])
    {
      callbacks[packetType](packetType, data, length, senderMac);
    }
  }

  bool clearCallback(uint8_t packetType) override
  {
    callbacks[packetType] = nullptr;
    return true;
  }

private:
  receiveCallback callbacks[256] = {nullptr};
};

#endif