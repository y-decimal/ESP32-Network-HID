#ifndef ESPNOWPROTOCOL_H
#define ESPNOWPROTOCOL_H

#include <shared/EventTypes.h>
#include <submodules/ConfigManager/ConfigManager.h>
#include <interfaces/IEspNow.h>
#include <functional>
#include <vector>
#include <unordered_map>

enum class PacketType : uint8_t
{
    KeyEvent,
    KeyBitmap,
    Config,
    PairingRequest,
    PairingConfirmation,
    Count
};

class EspNowProtocol
{
public:
    EspNowProtocol(IEspNow &espNow) : espNow(espNow) {};

    void sendKeyEvent(const RawKeyEvent &keyEvent);
    void sendBitmapEvent(const RawBitmapEvent &bitmapEvent);
    void requestConfig(const uint8_t id, ConfigManager *config);
    void pushConfig(const uint8_t id, ConfigManager *config);
    void sendPairingRequest(const uint8_t *data = nullptr);

    uint8_t getSelfId() const;
    void getMacById(uint8_t id, uint8_t *out) const;

    void onKeyEvent(std::function<void(const RawKeyEvent &keyEvent, uint8_t senderId)> callback);
    void onBitmapEvent(std::function<void(const RawBitmapEvent &bitmapEvent, uint8_t senderId)> callback);

    /**
     * @brief Allows registering custom hooks for received pairing requests.
     * @param data A pointer to the contents of the pairing request, a sequence number by default.
     * Only valid for the duration of the callback
     * @param sourceId The runtime ID assigned to the source upon pairing, can be used to address packets or
     * with getMacById(uint8_t id) to retrieve the mac of the device
     */
    void onPairingRequest(std::function<void(const uint8_t *data, const uint8_t sourceId)> callback);

    /**
     * @brief Allows registering custom hooks for received pairing confirmations.
     * @param data A pointer to the contents of the pairing confirmation, contains the request's
     * sequence number by default. Only valid for the duration of the callback
     * @param sourceId The runtime ID assigned to the source upon pairing, can be used to address packets or
     * with getMacById(uint8_t id) to retrieve the mac of the device
     */
    void onPairingConfirmation(std::function<void(const uint8_t *data, const uint8_t sourceId)> callback);

private:
    IEspNow &espNow;

    struct PeerInfo
    {
        uint8_t mac[6] = {0};
        uint8_t id = 0;
    };

    std::vector<PeerInfo> communicationPartners = {}; // List of active communication partners at runtime
};

#endif