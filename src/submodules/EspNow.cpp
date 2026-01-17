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
        {
            if (loggingEnabled)
                printf("[EspNow] Not initialized in sendData\n");
            return false;
        }

    if (!isMacRegistered(targetMac))
    {
        if (!registerCommPartner(targetMac))
        {
            if (loggingEnabled)
                printf("[EspNow] Could not register communication partner\n");
            return false;
        }
    }
    Header header;
    header.packetType = packetType;
    header.length = length;

    uint8_t buffer[sizeof(header) + length] = {};
    memcpy(buffer, &header, sizeof(header));
    memcpy(buffer + sizeof(header), data, length);

    esp_err_t sendSuccess = esp_now_send(targetMac, buffer, length + sizeof(header));
    if (sendSuccess != ESP_OK)
    {
        if (loggingEnabled)
            printf("[EspNow] esp_now_send failed\n");
        return false;
    }
    if (loggingEnabled)
        printf("[EspNow] Sent packet type %d to %02x:%02x:%02x:%02x:%02x:%02x\n",
               packetType,
               targetMac[0], targetMac[1], targetMac[2],
               targetMac[3], targetMac[4], targetMac[5]);
    return true;
}

bool EspNow::registerPacketTypeCallback(uint8_t packetType, receiveCallback callback)
{
    if (!initialized)
        if (!initialize())
        {
            if (loggingEnabled)
                printf("[EspNow] Not initialized in registerPacketTypeCallback\n");
            return false;
        }

    if (callbacks[packetType])
    {
        if (loggingEnabled)
            printf("[EspNow] Callback for packet type %d already registered\n", packetType);
        return false;
    }
    callbacks[packetType] = callback;
    if (loggingEnabled)
        printf("[EspNow] Registered callback for packet type %d\n", packetType);
    return true;
}

bool EspNow::clearCallback(uint8_t packetType)
{
    if (!callbacks[packetType])
    {
        if (loggingEnabled)
            printf("[EspNow] No callback to clear for packet type %d\n", packetType);
        return false;
    }
    callbacks[packetType] = nullptr;
    if (loggingEnabled)
        printf("[EspNow] Cleared callback for packet type %d\n", packetType);
    return true;
}

bool EspNow::initialize()
{
    if (esp_now_init() != ESP_OK)
    {
        if (loggingEnabled)
            printf("[EspNow] esp_now_init failed\n");
        return false;

    initialized = esp_now_register_recv_cb(routeCallback) == ESP_OK;

    return initialized;
}

bool EspNow::registerCommPartner(const uint8_t *mac)
{
    if (!initialized)
    {
        if (loggingEnabled)
            printf("[EspNow] Not initialized in registerCommPartner\n");
        return false;
    }

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
    {
        if (loggingEnabled)
            printf("[EspNow] Failed to add peer\n");
        return false;
    }
    if (loggingEnabled)
        printf("[EspNow] Registered peer successfully\n");
    return true;
}

bool EspNow::isMacRegistered(const uint8_t *mac)
{
    esp_now_peer_info_t peerInfo;
    esp_err_t peerFound = esp_now_get_peer(mac, &peerInfo);
    if (loggingEnabled)
        printf("[EspNow] Peer %02x:%02x:%02x:%02x:%02x:%02x %s registered\n",
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
               peerFound == ESP_OK ? "is" : "is not");

    return peerFound == ESP_OK;
}

void EspNow::routeCallback(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
    if (!instance)
    {
        printf("[EspNow] No instance in routeCallback\n");
        return;
    }

    Header header = {};
    memcpy(&header, data, sizeof(header));

    size_t totalPacketLength = data_len;
    size_t headerLength = sizeof(header);
    size_t dataLength = header.length;

    if (instance->loggingEnabled)
        printf("[EspNow] Received Packet: %d\n", header.packetType);

    if (dataLength != totalPacketLength - headerLength)
    {
        if (instance->loggingEnabled)
            printf("[EspNow] Invalid packet length: expected %d got %d \n", header.length, (data_len - sizeof(header)));
        return;
    }

    uint8_t buffer[dataLength] = {};
    memcpy(buffer, data, dataLength);

    if (instance->callbacks[header.packetType])
    {
        if (instance->loggingEnabled)
        {
            printf("[EspNow] Routing to callback for packet type %d\n", header.packetType);
            instance->callbacks[header.packetType](buffer, (size_t)dataLength, mac_addr);
        }
    }
    else if (instance->loggingEnabled)
        printf("[EspNow] No instance or callback found\n");
}