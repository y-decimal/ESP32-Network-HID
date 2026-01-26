#include <interfaces/implementations/HID/BleHidOutput.h>
#include <BLEDevice.h>
#include <interfaces/implementations/HID/Helper6KRO.h>

#include <submodules/Logger.h>

static Logger log(BleHidOutput::NAMESPACE);

bool BleHidOutput::initialize()
{
    log.info("Initializing BLE HID Output");
    BLEDevice::init("Decimator"); // Name in honor of someone's insistance I call it that

    BLEServer *server = BLEDevice::createServer();
    hid = new BLEHIDDevice(server);

    if (server == nullptr || hid == nullptr)
    {
        log.error("Failed to create BLE server or HID device");
        return false;
    }

    inputReport = hid->inputReport(1);
    outputReport = hid->outputReport(1);

    hid->manufacturer()->setValue("Decimal");

    // HID Information characteristic
    hid->pnp(0x02, 0x1234, 0x5678, 0x0100); // What does this mean?
    hid->hidInfo(0x00, 0x01);

    // Set the HID report descriptor
    hid->reportMap(sixKro.reportDescriptor6KRO, sizeof(sixKro.reportDescriptor6KRO));
    hid->startServices();
    BLEAdvertising *advertising = server->getAdvertising();
    advertising->setAppearance(HID_KEYBOARD);
    advertising->addServiceUUID(hid->hidService()->getUUID());
    advertising->start();

    return true;
}

void BleHidOutput::sendHidReport(const uint8_t *bitmap, size_t size)
{
    uint8_t report[8]{};
    sixKro.convertBitmapTo6KRO(bitmap, size, report);
    inputReport->setValue(report, sizeof(report));
    inputReport->notify();
}

BleHidOutput::KroMode BleHidOutput::getKroMode()
{
    return KroMode::MODE_6KRO;
}