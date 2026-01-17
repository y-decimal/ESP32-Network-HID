#include <submodules/EspNow.h>

bool EspNow::sendData(uint8_t packetType, uint8_t *data, size_t length, uint8_t targetMac[6])
{
    if (!initialized)
        if (!initialize())
            return false;

    if (!isMacRegistered(targetMac))
        if (!registerCommPartner(targetMac))
            return false;

    Header header;
    header.packetType = packetType;
    header.length = length;

    uint8_t buffer[sizeof(header) + length] = {};
    memcpy(buffer, &header, sizeof(header));
    memcpy(buffer + sizeof(header), data, length);

    esp_err_t sendSuccess = esp_now_send(targetMac, buffer, length + sizeof(packetType));
    if (sendSuccess != ESP_NOW_SEND_SUCCESS)
        return false;
    return true;
}

bool EspNow::registerPacketTypeCallback(uint8_t packetType, receiveCallback callback)
{
    if (!initialized)
        if (!initialize())
            return false;

    if (callbacks[packetType])
        return false;
    callbacks[packetType] = callback;
    return true;
}

bool EspNow::clearCallback(uint8_t packetType)
{
    if (!callbacks[packetType])
        return false;
    callbacks[packetType] = nullptr;
    return true;
}

bool EspNow::initialize()
{
    if (esp_now_init() != ESP_OK)
        return false;

    esp_err_t registerReceiveSuccess = esp_now_register_recv_cb(routeCallback);

    return registerReceiveSuccess == ESP_OK;
}