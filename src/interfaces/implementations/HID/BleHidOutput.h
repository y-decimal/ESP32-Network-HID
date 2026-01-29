#ifndef BLEHIDOUTPUT_H
#define BLEHIDOUTPUT_H

#include <interfaces/IHidOutput.h>
#include <submodules/HID/Helper6KRO.h>
#include <submodules/HID/HelperMediaKeys.h>
#include <BLEHIDDevice.h>

class BleHidOutput : public IHidOutput
{
public:
    static constexpr const char *NAMESPACE = "BleHidOutput";

    bool initialize() override;
    void sendHidReport(const uint8_t *hidBitmap, size_t size) override;
    KroMode getKroMode() override;

    void setConnected(bool state) { connected = state; }
    void setAuthenticated(bool state) { authenticated = state; }

private:
    BLEHIDDevice *hid;
    BLECharacteristic *inputReportKeyboard;
    BLECharacteristic *inputReportMediaControls;
    BLECharacteristic *outputReport;
    SixKroHelper sixKro;
    MediaKeysHelper mediaKeys;
    BLEServer *server;
    bool connected = false;
    bool authenticated = false;
};

#endif
