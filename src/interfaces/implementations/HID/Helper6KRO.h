#ifndef HELPER6KRO_H
#define HELPER6KRO_H

#include <HIDTypes.h>
#include <HIDKeyboardTypes.h>
#include <string>
#include <vector>
#include <cstring>

/**
 * Modifier masks - used for the first byte in the HID report.
 * NOTE: The second byte in the report is reserved, 0x00
 */
#define KEY_MOD_LCTRL 0x01
#define KEY_MOD_LSHIFT 0x02
#define KEY_MOD_LALT 0x04
#define KEY_MOD_LMETA 0x08
#define KEY_MOD_RCTRL 0x10
#define KEY_MOD_RSHIFT 0x20
#define KEY_MOD_RALT 0x40
#define KEY_MOD_RMETA 0x80

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
};

#endif