#include <submodules/TransportProtocol.h>

static constexpr uint8_t KEY_EVENT = static_cast<uint8_t>(PacketType::KeyEvent);
static constexpr uint8_t KEY_BITMAP = static_cast<uint8_t>(PacketType::KeyBitmap);
static constexpr uint8_t CONFIG_REQUEST = static_cast<uint8_t>(PacketType::ConfigRequest);
static constexpr uint8_t CONFIG = static_cast<uint8_t>(PacketType::Config);
static constexpr uint8_t PAIRING_REQUEST = static_cast<uint8_t>(PacketType::PairingRequest);
static constexpr uint8_t PAIRING_CONFIRMATION = static_cast<uint8_t>(PacketType::PairingConfirmation);

static constexpr uint8_t BROADCASTMAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static constexpr uint8_t NULLMAC[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

TransportProtocol::TransportProtocol(ITransport &espNow)
    : transport(espNow)
{
    peerDevices.push_back({}); // Reserve ID 0 for master
    transport.registerPacketTypeCallback(PAIRING_REQUEST,
                                         [this](uint8_t type, const uint8_t *data, size_t len, const uint8_t *mac)
                                         {
                                             this->handlePairingRequest(data, len, mac);
                                         });
    transport.registerPacketTypeCallback(PAIRING_CONFIRMATION,
                                         [this](uint8_t type, const uint8_t *data, size_t len, const uint8_t *mac)
                                         {
                                             this->handlePairingConfirmation(data, len, mac);
                                         });
}

void TransportProtocol::sendKeyEvent(const RawKeyEvent &keyEvent)
{
    size_t len = sizeof(RawKeyEvent);
    uint8_t *buffer = (uint8_t *)malloc(len);
    memcpy(buffer, &keyEvent, len);
    transport.sendData(KEY_EVENT, buffer, len, masterMac.data());
    free(buffer);
}

void TransportProtocol::sendBitmapEvent(const RawBitmapEvent &bitmapEvent)
{
    size_t len = sizeof(RawKeyEvent);
    uint8_t *buffer = (uint8_t *)malloc(len);
    memcpy(buffer, &bitmapEvent, len);
    transport.sendData(KEY_BITMAP, buffer, len, masterMac.data());
    free(buffer);
}

void TransportProtocol::requestConfig(uint8_t id)
{
    uint8_t emptyPacket = 0;
    mac_t mac = {};
    getMacById(id, mac.data());
    transport.sendData(CONFIG_REQUEST, &emptyPacket, sizeof(emptyPacket), mac.data());
}

void TransportProtocol::pushConfig(uint8_t id, const ConfigManager *config)
{
    size_t len = sizeof(ConfigManager);
    uint8_t *buffer = (uint8_t *)malloc(len);
    memcpy(buffer, config, len);

    mac_t mac = {};
    getMacById(id, mac.data());
    transport.sendData(CONFIG, buffer, len, mac.data());
    free(buffer);
}

void TransportProtocol::sendPairingRequest(const uint8_t *data, size_t dataLen)
{
    uint8_t *pairingPacket;
    size_t len = dataLen;
    if (data)
        pairingPacket = const_cast<uint8_t *>(data);
    else
    {
        pairingPacket = new uint8_t(0);
        len = 1;
    }

    transport.sendData(PAIRING_REQUEST, pairingPacket, len, masterMac);
}

uint8_t TransportProtocol::getSelfId() const
{
    return 0;
}

void TransportProtocol::getMacById(uint8_t id, uint8_t *out) const
{
    memcpy(out, peerDevices.at(id).data(), sizeof(mac_t));
}

uint8_t TransportProtocol::getIdByMac(const uint8_t *mac) const
{
    size_t macSize = sizeof(mac_t);
    for (int i = 0; i < peerDevices.size(); i++)
    {
        if (memcmp(mac, peerDevices[i].data(), macSize) == 0)
            return i;
    }
    return 0xFF;
}

void TransportProtocol::onKeyEvent(std::function<void(const RawKeyEvent &keyEvent, uint8_t senderId)> callback)
{
    keyEventCallback = callback;
    transport.registerPacketTypeCallback(KEY_EVENT,
                                         [this](uint8_t type, const uint8_t *data, size_t len, const uint8_t *mac)
                                         {
                                             if (keyEventCallback && len >= sizeof(RawKeyEvent))
                                             {
                                                 RawKeyEvent keyEvent;
                                                 memcpy(&keyEvent, data, sizeof(RawKeyEvent));
                                                 keyEventCallback(keyEvent, getIdByMac(mac));
                                             }
                                         });
}

void TransportProtocol::onBitmapEvent(std::function<void(const RawBitmapEvent &bitmapEvent, uint8_t senderId)> callback)
{
    bitmapEventCallback = callback;
    transport.registerPacketTypeCallback(KEY_BITMAP,
                                         [this](uint8_t type, const uint8_t *data, size_t len, const uint8_t *mac)
                                         {
                                             if (bitmapEventCallback && len >= sizeof(RawBitmapEvent))
                                             {
                                                 RawBitmapEvent bitmapEvent;
                                                 memcpy(&bitmapEvent, data, sizeof(RawBitmapEvent));
                                                 bitmapEventCallback(bitmapEvent, getIdByMac(mac));
                                             }
                                         });
}

void TransportProtocol::onConfigReceived(std::function<void(ConfigManager &config, uint8_t senderId)> callback)
{
    configCallback = callback;
    transport.registerPacketTypeCallback(CONFIG,
                                         [this](uint8_t type, const uint8_t *data, size_t len, const uint8_t *mac)
                                         {
                                             if (configCallback && len >= sizeof(ConfigManager))
                                             {
                                                 ConfigManager config;
                                                 memcpy(&config, data, sizeof(ConfigManager));
                                                 configCallback(config, getIdByMac(mac));
                                             }
                                         });
}

void TransportProtocol::onPairingRequest(std::function<void(const uint8_t *data, uint8_t sourceId)> callback)
{
    pairingRequestCallback = callback;
    transport.registerPacketTypeCallback(PAIRING_REQUEST,
                                         [this](uint8_t type, const uint8_t *data, size_t len, const uint8_t *mac)
                                         {
                                             peerDevices.push_back({});
                                             memcpy(peerDevices.back(), mac, sizeof(mac_t));
                                             this->transport.sendData(PAIRING_CONFIRMATION, data, len, mac);
                                             if (pairingRequestCallback)
                                             {
                                                 pairingRequestCallback(data, getIdByMac(mac));
                                             }
                                         });
}

void TransportProtocol::onPairingConfirmation(std::function<void(const uint8_t *data, uint8_t sourceId)> callback)
{
    pairingConfirmationCallback = callback;
    transport.registerPacketTypeCallback(PAIRING_CONFIRMATION,
                                         [this](uint8_t type, const uint8_t *data, size_t len, const uint8_t *mac)
                                         {
                                             peerDevices.push_back({});
                                             memcpy(peerDevices.back(), mac, sizeof(mac_t));
                                             if (pairingConfirmationCallback)
                                             {
                                                 pairingConfirmationCallback(data, getIdByMac(mac));
                                             }
                                         });
}

// void TransportProtocol::