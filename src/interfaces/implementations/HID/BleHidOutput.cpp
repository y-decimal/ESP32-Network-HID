#include <interfaces/implementations/HID/BleHidOutput.h>
#include <BLEDevice.h>
#include <BLESecurity.h>
#include <interfaces/implementations/HID/Helper6KRO.h>

#include <submodules/Logger.h>

static Logger log(BleHidOutput::NAMESPACE);

class MySecurityCallbacks : public BLESecurityCallbacks
{
    BleHidOutput *hidOutput;

public:
    MySecurityCallbacks(BleHidOutput *output) : hidOutput(output) {}

    uint32_t onPassKeyRequest() override { return 0; }
    void onPassKeyNotify(uint32_t pass_key) override {}
    bool onConfirmPIN(uint32_t pass_key) override { return true; }
    bool onSecurityRequest() override { return true; }
    void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl) override
    {
        if (cmpl.success)
        {
            log.info("BLE authentication successful");
            hidOutput->setAuthenticated(true);
        }
        else
        {
            log.warn("BLE authentication failed");
            hidOutput->setAuthenticated(false);
        }
    }
};

class MyServerCallbacks : public BLEServerCallbacks
{
    BleHidOutput *hidOutput;

public:
    MyServerCallbacks(BleHidOutput *output) : hidOutput(output) {}

    void onConnect(BLEServer *pServer) override
    {
        log.info("BLE client connected");
        hidOutput->setConnected(true);
    }

    void onDisconnect(BLEServer *pServer) override
    {
        log.info("BLE client disconnected");
        hidOutput->setConnected(false);
        hidOutput->setAuthenticated(false);

        // Restart advertising
        BLEAdvertising *advertising = pServer->getAdvertising();
        advertising->start();
        log.info("Restarted advertising");
    }
};

bool BleHidOutput::initialize()
{
    log.info("Initializing BLE HID Output");
    BLEDevice::init("Decimator"); // Name in honor of someone's insistance I call it that

    // Set BLE security - HID requires encryption and bonding
    BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
    BLEDevice::setSecurityCallbacks(new MySecurityCallbacks(this));

    BLESecurity *pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND); // Secure connections with MITM protection and bonding
    pSecurity->setCapability(ESP_IO_CAP_NONE);                      // No display, no input
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

    server = BLEDevice::createServer();
    server->setCallbacks(new MyServerCallbacks(this));
    hid = new BLEHIDDevice(server);

    if (server == nullptr || hid == nullptr)
    {
        log.error("Failed to create BLE server or HID device");
        return false;
    }

    inputReportKeyboard = hid->inputReport(1);
    inputReportMediaControls = hid->inputReport(2);
    outputReport = hid->outputReport(1);

    hid->manufacturer()->setValue("Decimal");

    // HID Information characteristic
    hid->pnp(0x02, 0x1234, 0x5678, 0x0100); // What does this mean?
    hid->hidInfo(0x00, 0x01);

    // Set the HID report descriptor
    hid->reportMap(sixKro.reportDescriptor6KRO, sizeof(sixKro.reportDescriptor6KRO));
    hid->startServices();

    // Set security on HID service
    BLECharacteristic *inputChar = inputReportKeyboard;
    BLECharacteristic *inputCharMedia = inputReportMediaControls;
    BLECharacteristic *outputChar = outputReport;
    if (inputChar)
    {
        inputChar->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    }
    if (inputCharMedia)
    {
        inputCharMedia->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    }
    if (outputChar)
    {
        outputChar->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    }

    BLEAdvertising *advertising = server->getAdvertising();
    advertising->setAppearance(HID_KEYBOARD);
    advertising->addServiceUUID(hid->hidService()->getUUID());
    advertising->start();

    log.info("BLE HID initialized with security enabled");
    return true;
}

void BleHidOutput::sendHidReport(const uint8_t *bitmap, size_t size)
{
    if (!connected || !authenticated)
    {
        log.debug("Not sending report: connected=%d, authenticated=%d", connected, authenticated);
        return;
    }

    log.debug("sendHidReport: Entry, bitmap=%d, size=%zu", *bitmap, size);

    uint8_t report[8]{0};
    log.debug("sendHidReport: Converting bitmap to 6KRO");
    sixKro.convertBitmapTo6KRO(bitmap, size, report);

    std::string debugStr = "HID Report: ";
    for (size_t i = 0; i < sizeof(report); i++)
    {
        debugStr += " " + std::to_string(report[i]);
        ;
    }
    log.debug("%s", debugStr.c_str());

    log.debug("sendHidReport: Setting input report");
    inputReportKeyboard->setValue(report, sizeof(report));

    log.debug("sendHidReport: Calling notify()");
    inputReportKeyboard->notify();

    log.debug("sendHidReport: Notify completed, exiting");
}

BleHidOutput::KroMode BleHidOutput::getKroMode()
{
    return KroMode::MODE_6KRO;
}