#ifndef ESPNOW_H
#define ESPNOW_H

#include <interfaces/IEspNow.h>
#include <esp_now.h>
#include <vector>
#include <cstring>

class EspNow : IEspNow
{
public:
    bool sendData(uint8_t packetType, uint8_t *data, size_t length, uint8_t targetMac[6]) override;
    bool registerPacketTypeCallback(uint8_t packetType, receiveCallback callback) override;
    bool clearCallback(uint8_t packetType) override;

private:
    receiveCallback callbacks[UINT8_MAX];

    struct Header
    {
        uint8_t packetType;
        size_t length;
    };

    void registerEspNowReceiveCallback();
    bool registerCommPartner(uint8_t *mac);

    bool isMacRegistered(uint8_t *mac);

    void routeCallback(uint8_t packetType);
};

#endif