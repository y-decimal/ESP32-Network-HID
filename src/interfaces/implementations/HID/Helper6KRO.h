#ifndef HELPER6KRO_H
#define HELPER6KRO_H

#include <HIDTypes.h>
#include <HIDKeyboardTypes.h>
#include <string>
#include <vector>
#include <cstring>

/**
 * HID Usage codes for modifier keys (appear in bitmap)
 */
#define HID_KEY_LCTRL 0xE0
#define HID_KEY_LSHIFT 0xE1
#define HID_KEY_LALT 0xE2
#define HID_KEY_LMETA 0xE3
#define HID_KEY_RCTRL 0xE4
#define HID_KEY_RSHIFT 0xE5
#define HID_KEY_RALT 0xE6
#define HID_KEY_RMETA 0xE7

class SixKroHelper
{
public:
    static constexpr const char *NAMESPACE = "SixKroHelper";

    void convertBitmapTo6KRO(const uint8_t *bitmap, size_t bitmapSize, uint8_t *outputReport);

    static uint8_t reportDescriptor6KRO[63];

private:
    std::vector<uint8_t> pressedKeysInOrder;
    uint8_t *lastBitmap = nullptr;

    bool isModifier(uint8_t hidCode);
    bool isKeyboardUsage(uint8_t hidCode);
};

#endif