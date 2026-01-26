#include <interfaces/implementations/HID/BleHidOutput.h>
#include <BLEDevice.h>
#include <interfaces/implementations/HID/Helper6KRO.h>

bool BleHidOutput::initialize()
{
    BLEDevice::init("Decimator"); // Name in honor of someone's insistance I call it that

    BLEServer *server = BLEDevice::createServer();
    hid = new BLEHIDDevice(server);

    inputReport = hid->inputReport(1);
    outputReport = hid->outputReport(1);

    hid->manufacturer()->setValue("Decimal");

    // HID Information characteristic
    hid->pnp(0x02, 0x1234, 0x5678, 0x0100); // What does this mean?
    hid->hidInfo(0x00, 0x01);

    // Set the HID report descriptor (we’ll fill this in next)
    hid->reportMap(sixKro.reportDescriptor6KRO, sizeof(sixKro.reportDescriptor6KRO));
    hid->startServices();
    BLEAdvertising *advertising = server->getAdvertising();
    advertising->setAppearance(HID_KEYBOARD);
    advertising->addServiceUUID(hid->hidService()->getUUID());
    advertising->start();
}

void BleHidOutput::sendHidReport(const uint8_t *bitmap, size_t size)
{
    uint8_t report[8]{};
    sixKro.convertBitmapTo6KRO(bitmap, size, report);
    inputReport->setValue(report, sizeof(report));
    inputReport->notify();
}
