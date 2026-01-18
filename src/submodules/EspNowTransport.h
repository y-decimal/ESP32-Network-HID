#ifndef ESPNOWTRANSPORT_H
#define ESPNOWTRANSPORT_H

#include <interfaces/IEspNow.h>
#include <esp_now.h>
#include <vector>
#include <cstring>

class EspNow : public IEspNow
{
public:
    EspNow();
    bool sendData(uint8_t packetType, const uint8_t *data, size_t length, const uint8_t *targetMac) override;
    bool registerPacketTypeCallback(uint8_t packetType, receiveCallback callback) override;
    bool clearCallback(uint8_t packetType) override;

private:
    receiveCallback callbacks[256] = {nullptr};
    bool initialized = false;
    static EspNow *instance;

    bool loggingEnabled = false;

    struct Header
    {
        uint8_t packetType;
        size_t length;
    };

    bool initialize();
    bool registerCommPartner(const uint8_t *mac);
    bool isMacRegistered(const uint8_t *mac);

    static void routeCallback(const uint8_t *mac_addr, const uint8_t *data, int data_len);
    static void espNowSendCallback(const uint8_t *mac_addr, esp_now_send_status_t status);
};

#endif