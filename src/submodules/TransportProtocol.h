#ifndef ESPNOWPROTOCOL_H
#define ESPNOWPROTOCOL_H

#include <shared/EventTypes.h>
#include <submodules/ConfigManager/ConfigManager.h>
#include <interfaces/ITransport.h>
#include <functional>
#include <vector>
#include <unordered_map>

enum class PacketType : uint8_t
{
    KeyEvent,
    KeyBitmap,
    Config,
    ConfigRequest,
    PairingRequest,
    PairingConfirmation,
    Count
};

class TransportProtocol
{
public:
    static const uint8_t MASTER_ID = 0;

    TransportProtocol(ITransport &espNow) : transport(espNow) {};

    void sendKeyEvent(const RawKeyEvent &keyEvent);
    void sendBitmapEvent(const RawBitmapEvent &bitmapEvent);
    void requestConfig(uint8_t id);
    void pushConfig(uint8_t id, const ConfigManager *config);
    void sendPairingRequest(const uint8_t *data = nullptr, size_t dataLen = 0);

    uint8_t getSelfId() const;
    void getMacById(uint8_t id, uint8_t *out) const;

    void onKeyEvent(std::function<void(const RawKeyEvent &keyEvent, uint8_t senderId)> callback);
    void onBitmapEvent(std::function<void(const RawBitmapEvent &bitmapEvent, uint8_t senderId)> callback);
    void onConfigReceived(std::function<void(ConfigManager &config, uint8_t senderId)> callback);

    /**
     * @brief Allows registering custom hooks for received pairing requests.
     * @param data A pointer to the contents of the pairing request, a sequence number by default.
     * Only valid for the duration of the callback
     * @param sourceId The runtime ID assigned to the source upon pairing, can be used to address packets or
     * with getMacById(uint8_t id) to retrieve the mac of the device
     */
    void onPairingRequest(std::function<void(const uint8_t *data, uint8_t sourceId)> callback);

    /**
     * @brief Allows registering custom hooks for received pairing confirmations.
     * @param data A pointer to the contents of the pairing confirmation, contains the request's
     * sequence number by default. Only valid for the duration of the callback
     * @param sourceId The runtime ID assigned to the source upon pairing, can be used to address packets or
     * with getMacById(uint8_t id) to retrieve the mac of the device
     */
    void onPairingConfirmation(std::function<void(const uint8_t *data, uint8_t sourceId)> callback);

private:
    typedef uint8_t mac_t[6];

    ITransport &transport;

    std::vector<mac_t> communicationPartners = {}; // List of active communication partners at runtime
    std::unordered_map<mac_t, uint8_t> macToIdMap;
    mac_t masterMac = {};
};

#endif