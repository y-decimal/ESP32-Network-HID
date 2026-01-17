#include <submodules/EspNow.h>

EspNow *EspNow::instance = nullptr;

EspNow::EspNow()
{
    instance = this;
}

bool EspNow::sendData(uint8_t packetType, const uint8_t *data, size_t length, const uint8_t *targetMac)
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

    esp_err_t sendSuccess = esp_now_send(targetMac, buffer, length + sizeof(header));
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

    initialized = esp_now_register_recv_cb(routeCallback) == ESP_OK;

    return initialized;
}

bool EspNow::registerCommPartner(const uint8_t *mac)
{
    if (!initialized)
        return false;

    esp_now_peer_info_t peerInfo = {};
    peerInfo.channel = 1; // Use channel 1 instead of 0

    uint8_t broadcast[6] = {255, 255, 255, 255, 255};
    if (memcmp(mac, broadcast, 6) == 0)
        peerInfo.encrypt = 0;
    else
        peerInfo.encrypt = 1;
    memcpy(peerInfo.peer_addr, mac, 6);

    esp_err_t addPeerSuccess = esp_now_add_peer(&peerInfo);

    if (addPeerSuccess != ESP_OK)
        return false;

    return true;
}

bool EspNow::isMacRegistered(const uint8_t *mac)
{
    esp_now_peer_info_t peerInfo;
    esp_err_t peerFound = esp_now_get_peer(mac, &peerInfo);

    return peerFound == ESP_OK;
}

void EspNow::routeCallback(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
    Header header = {};
    memcpy(&header, data, sizeof(header));

    size_t totalPacketLength = data_len;
    size_t headerLength = sizeof(header);
    size_t dataLength = header.length;

    printf("[EspNow] Received Packet: %d\n", header.packetType);

    if (dataLength != totalPacketLength - headerLength)
    {
        printf("[EspNow] Invalid packet length: expected %d got %d \n", header.length, (data_len - sizeof(header)));
        return;
    }

    uint8_t buffer[dataLength] = {};
    memcpy(buffer, data, dataLength);

    if (instance && instance->callbacks[header.packetType])
    {
        printf("[EspNow] Calling callback for type %d\n", header.packetType);
        instance->callbacks[header.packetType](buffer, (size_t)dataLength, mac_addr);
    }
    else
        printf("[EspNow] No instance or callback found\n");
}