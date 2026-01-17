#ifndef IESP_NOW_H
#define IESP_NOW_H

#include <functional>
#include <stdint.h>

class IEspNow
{
public:
    using receiveCallback = std::function<void(const uint8_t *data, size_t length, const uint8_t *senderMac)>;

    virtual bool sendData(uint8_t packetType, const uint8_t *data, size_t length, const uint8_t *targetMac) = 0;
    virtual bool registerPacketTypeCallback(uint8_t packetType, receiveCallback callback) = 0;
    virtual bool clearCallback(uint8_t PacketType) = 0;

    // Virtual destructor
    virtual ~IEspNow() = default;
};

#endif