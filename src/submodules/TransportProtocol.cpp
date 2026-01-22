#include <submodules/TransportProtocol.h>
#include <submodules/Logger.h>

static Logger log(__FILENAME__);

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
    log.info("Sending Key Event to Master");
    size_t len = sizeof(RawKeyEvent);
    uint8_t *buffer = (uint8_t *)malloc(len);
    memcpy(buffer, &keyEvent, len);
    transport.sendData(KEY_EVENT, buffer, len, masterMac.data());
    free(buffer);
}

void TransportProtocol::sendBitmapEvent(const RawBitmapEvent &bitmapEvent)
{
    log.debug("Sending Bitmap Event to Master");
    // Serialize as: [bitmapSize (1 byte)][bitMapData (N bytes)]
    size_t len = 1 + bitmapEvent.bitmapSize;
    uint8_t *buffer = (uint8_t *)malloc(len);
    buffer[0] = bitmapEvent.bitmapSize;
    memcpy(buffer + 1, bitmapEvent.bitMapData, bitmapEvent.bitmapSize);
    transport.sendData(KEY_BITMAP, buffer, len, masterMac.data());
    free(buffer);
}

void TransportProtocol::requestConfig(uint8_t id)
{
    log.info("Requesting Config from ID %d", id);
    uint8_t emptyPacket = 0;
    mac_t mac = {};
    getMacById(id, mac.data());
    transport.sendData(CONFIG_REQUEST, &emptyPacket, sizeof(emptyPacket), mac.data());
}

void TransportProtocol::sendConfig(uint8_t id, const ConfigManager *config)
{
    log.info("Sending config to ID %d", id);

    size_t requiredSize = config->getSerializedSize();
    uint8_t *buffer = (uint8_t *)malloc(requiredSize);
    size_t len = config->packSerialized(buffer, requiredSize);
    if (len == 0 || len != requiredSize)
    {
        log.error("Failed to serialize config for sending to ID %d: expected %zu, got %zu", id, requiredSize, len);
        free(buffer);
        return;
    }

    mac_t mac = {};
    getMacById(id, mac.data());

    transport.sendData(CONFIG, buffer, len, mac.data());
    free(buffer);
}

void TransportProtocol::sendPairingRequest(const uint8_t *data, size_t dataLen)
{
    log.debug("Sending pairing request");
    uint8_t emptyPacket = 0;
    const uint8_t *pairingPacket = data ? data : &emptyPacket;
    size_t len = data ? dataLen : 1;

    transport.sendData(PAIRING_REQUEST, pairingPacket, len, BROADCASTMAC);
}

uint8_t TransportProtocol::getSelfId() const
{
    return 0;
}

void TransportProtocol::getMacById(uint8_t id, uint8_t *out) const
{
    if (id >= peerDevices.size())
    {
        log.error("Invalid ID %d (max: %d)", id, peerDevices.size() - 1);
        memcpy(out, NULLMAC, sizeof(mac_t));
        return;
    }

    const mac_t &mac = peerDevices[id];

    // Check if MAC is NULL
    if (memcmp(mac.data(), NULLMAC, sizeof(mac_t)) == 0)
    {
        log.error("ID %d has NULL MAC address", id);
    }

    memcpy(out, mac.data(), sizeof(mac_t));
}

uint8_t TransportProtocol::getIdByMac(const uint8_t *mac) const
{
    size_t macSize = sizeof(mac_t);
    for (int i = 0; i < peerDevices.size(); i++)
    {
        if (memcmp(mac, peerDevices[i].data(), macSize) == 0)
            return i;
    }
    log.debug("MAC not found in peerDevices: %02X:%02X:%02X:%02X:%02X:%02X",
              mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return 0xFF;
}

void TransportProtocol::onKeyEvent(std::function<void(const RawKeyEvent &keyEvent, uint8_t senderId)> callback)
{
    keyEventCallback = callback;
    transport.registerPacketTypeCallback(KEY_EVENT,
                                         [this](uint8_t type, const uint8_t *data, size_t len, const uint8_t *mac)
                                         { handleKeyEventData(data, len, mac); });
    log.info("Registered onKeyEvent callback");
}

void TransportProtocol::onBitmapEvent(std::function<void(const RawBitmapEvent &bitmapEvent, uint8_t senderId)> callback)
{
    bitmapEventCallback = callback;
    transport.registerPacketTypeCallback(KEY_BITMAP,
                                         [this](uint8_t type, const uint8_t *data, size_t len, const uint8_t *mac)
                                         { handleBitmapEventData(data, len, mac); });
    log.info("Registered onBitmapEvent callback");
}

void TransportProtocol::onConfigReceived(std::function<void(const ConfigManager &config, uint8_t senderId)> callback)
{
    configCallback = callback;
    transport.registerPacketTypeCallback(CONFIG,
                                         [this](uint8_t type, const uint8_t *data, size_t len, const uint8_t *mac)
                                         { handleConfigData(data, len, mac); });
    log.info("Registered onConfigReceived callback");
}

void TransportProtocol::onConfigRequest(std::function<void(uint8_t sourceId)> callback)
{
    configRequestCallback = callback;
    transport.registerPacketTypeCallback(CONFIG_REQUEST,
                                         [this](uint8_t type, const uint8_t *data, size_t len, const uint8_t *mac)
                                         {
                                             if (getIdByMac(mac) == 0xFF)
                                             {
                                                 peerDevices.push_back({});
                                                 memcpy(peerDevices.back().data(), mac, sizeof(mac_t));
                                             }
                                             if (configRequestCallback)
                                             {
                                                 configRequestCallback(getIdByMac(mac));
                                             }
                                         });
    log.info("Registered onConfigRequest callback");
}

void TransportProtocol::onPairingRequest(std::function<void(uint8_t sourceId)> callback)
{
    pairingRequestCallback = callback;
    log.info("Registered onPairingRequest callback");
}

void TransportProtocol::onPairingConfirmation(std::function<void(uint8_t sourceId)> callback)
{
    pairingConfirmationCallback = callback;
    log.info("Registered onPairingConfirmation callback");
}

void TransportProtocol::handlePairingRequest(const uint8_t *data, size_t dataLen, const uint8_t *mac)
{
    if (getIdByMac(mac) == 0xFF)
    {
        log.info("Adding new device from pairing request");
        peerDevices.push_back({});
        memcpy(peerDevices.back().data(), mac, sizeof(mac_t));
    }
    else
        log.info("Device already known with ID %d", getIdByMac(mac));

    this->transport.sendData(PAIRING_CONFIRMATION, data, dataLen, mac);

    if (pairingRequestCallback)
    {
        pairingRequestCallback(getIdByMac(mac));
        log.info("Triggered pairing request callback for ID %d", getIdByMac(mac));
    }
}

void TransportProtocol::handlePairingConfirmation(const uint8_t *data, size_t dataLen, const uint8_t *mac)
{
    if (getIdByMac(mac) == 0xFF)
    {
        log.info("Adding new master device from pairing confirmation");
        peerDevices.push_back({});
        memcpy(peerDevices.back().data(), mac, sizeof(mac_t));
        memcpy(masterMac.data(), mac, sizeof(mac_t));
    }
    else
        log.info("Master device already known with ID %d", getIdByMac(mac));

    if (pairingConfirmationCallback)
    {
        pairingConfirmationCallback(getIdByMac(mac));
        log.info("Triggered pairing confirmation callback for ID %d", getIdByMac(mac));
    }
}

void TransportProtocol::clearCallbacks()
{
    keyEventCallback = nullptr;
    bitmapEventCallback = nullptr;
    configCallback = nullptr;
    pairingRequestCallback = nullptr;
    pairingConfirmationCallback = nullptr;
    log.info("Cleared all registered callbacks");
}

void TransportProtocol::handleKeyEventData(const uint8_t *data, size_t len, const uint8_t *mac)
{
    if (getIdByMac(mac) == 0xFF)
    {
        peerDevices.push_back({});
        memcpy(peerDevices.back().data(), mac, sizeof(mac_t));
    }
    if (keyEventCallback && len >= sizeof(RawKeyEvent))
    {
        RawKeyEvent keyEvent;
        memcpy(&keyEvent, data, sizeof(RawKeyEvent));
        keyEventCallback(keyEvent, getIdByMac(mac));
    }
    log.info("Received key event from ID %d", getIdByMac(mac));
}

void TransportProtocol::handleBitmapEventData(const uint8_t *data, size_t len, const uint8_t *mac)
{
    if (getIdByMac(mac) == 0xFF)
    {
        peerDevices.push_back({});
        memcpy(peerDevices.back().data(), mac, sizeof(mac_t));
    }
    // Deserialize: [bitmapSize (1 byte)][bitMapData (N bytes)]
    if (bitmapEventCallback && len >= 1)
    {
        RawBitmapEvent bitmapEvent;
        bitmapEvent.bitmapSize = data[0];
        if (len >= 1 + bitmapEvent.bitmapSize)
        {
            bitmapEvent.bitMapData = (uint8_t *)malloc(bitmapEvent.bitmapSize);
            memcpy(bitmapEvent.bitMapData, data + 1, bitmapEvent.bitmapSize);
            bitmapEventCallback(bitmapEvent, getIdByMac(mac));
        }
    }
    log.debug("Received bitmap event from ID %d", getIdByMac(mac));
}

void TransportProtocol::handleConfigData(const uint8_t *data, size_t len, const uint8_t *mac)
{
    if (getIdByMac(mac) == 0xFF)
    {
        peerDevices.push_back({});
        memcpy(peerDevices.back().data(), mac, sizeof(mac_t));
        log.info("Added new device from config data with ID %d", getIdByMac(mac));
    }

    // Basic sanity check - must have at least 2 size_t fields
    if (len < 2 * sizeof(size_t))
    {
        log.error("Received config data too small from ID %d (got %zu bytes)",
                  getIdByMac(mac), len);
        return;
    }

    if (configCallback)
    {
        ConfigManager config;
        size_t unpacked = config.unpackSerialized(data, len);
        if (unpacked == 0 || unpacked != len)
        {
            log.error("Failed to unpack config from ID %d (unpacked %zu of %zu bytes)",
                      getIdByMac(mac), unpacked, len);
            return;
        }
        configCallback(config, getIdByMac(mac));
        log.info("Triggered config callback for ID %d", getIdByMac(mac));
    }

    log.info("Received config from ID %d", getIdByMac(mac));
}