#ifndef IESP_NOW_H
#define IESP_NOW_H

#include <functional>
#include <stdint.h>

class IEspNow
{
public:
    using receiveCallback = std::function<void(uint8_t *data, size_t length, uint8_t senderMac[6])>;

    virtual bool sendData(uint8_t packetType, uint8_t *data, size_t length, uint8_t targetMac[6]) = 0;
    virtual bool registerPacketTypeCallback(uint8_t packetType, receiveCallback callback) = 0;
};

#endif