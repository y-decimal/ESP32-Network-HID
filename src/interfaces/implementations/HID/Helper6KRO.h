#ifndef HELPER6KRO_H
#define HELPER6KRO_H

#include <HIDTypes.h>
#include <HIDKeyboardTypes.h>
#include <string>

// Define HID report descriptor macros if not already defined
#ifndef INPUT
#define INPUT(x) 0x81, x
#endif
#ifndef OUTPUT
#define OUTPUT(x) 0x91, x
#endif

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
    void convertBitmapTo6KRO(const uint8_t *bitmap, size_t bitmapSize, uint8_t *outputReport)
    {
        if (lastBitmap == nullptr)
            lastBitmap = (uint8_t *)malloc(bitmapSize);
        memcpy(lastBitmap, bitmap, bitmapSize);

        uint8_t modifierByte = 0;

        for (uint8_t hidCode = 0; hidCode < bitmapSize * 8; hidCode++)
        {
            bool wasDown = lastBitmap[hidCode / 8] & (1 << (hidCode % 8));
            bool isDown = bitmap[hidCode / 8] & (1 << (hidCode % 8));

            if (!wasDown && isDown)
            {
                if (isModifier(hidCode))
                    modifierByte |= hidCode;
                else
                    pressedKeysInOrder.push_back(hidCode);
            }
            else if (wasDown && !isDown)
            {
                if (isModifier(hidCode))
                    modifierByte &= !hidCode;
                else
                    for (int i = 0; i < pressedKeysInOrder.size(); i++)
                    {
                        if (pressedKeysInOrder[i] == hidCode)
                            pressedKeysInOrder.erase(pressedKeysInOrder.begin() + i);
                    }
            }
        }

        outputReport[0] = modifierByte;
        outputReport[1] = 0;

        size_t count = 0;
        for (uint8_t hidCode : pressedKeysInOrder)
        {
            if (isModifier(hidCode))
                continue;
            if (count >= 6)
                break;

            outputReport[2 + count] = hidCode;
            count++;
        }
    }

    static uint8_t reportDescriptor6KRO[];

private:
    std::vector<uint8_t> pressedKeysInOrder;
    uint8_t *lastBitmap = nullptr;

    bool isModifier(uint8_t hidCode)
    {
        switch (hidCode)
        {
        case KEY_MOD_LALT:
            return true;
        case KEY_MOD_LCTRL:
            return true;
        case KEY_MOD_LMETA:
            return true;
        case KEY_MOD_LSHIFT:
            return true;
        case KEY_MOD_RALT:
            return true;
        case KEY_MOD_RCTRL:
            return true;
        case KEY_MOD_RMETA:
            return true;
        case KEY_MOD_RSHIFT:
            return true;
        default:
            return false;
        }
    }
};

uint8_t SixKroHelper::reportDescriptor6KRO[] = {
    USAGE_PAGE(1),
    0x01, // Generic Desktop
    USAGE(1),
    0x06, // Keyboard
    COLLECTION(1),
    0x01, // Application

    // Modifier byte
    REPORT_ID(1),
    0x01,
    USAGE_PAGE(1),
    0x07, // Keyboard/Keypad
    USAGE_MINIMUM(1),
    0xE0,
    USAGE_MAXIMUM(1),
    0xE7,
    LOGICAL_MINIMUM(1),
    0x00,
    LOGICAL_MAXIMUM(1),
    0x01,
    REPORT_SIZE(1),
    0x01,
    REPORT_COUNT(1),
    0x08,
    INPUT(1),
    0x02, // Data, Variable, Absolute

    // Reserved byte
    REPORT_COUNT(1),
    0x01,
    REPORT_SIZE(1),
    0x08,
    INPUT(1),
    0x01, // Constant

    // 6KRO key array
    REPORT_COUNT(1),
    0x06,
    REPORT_SIZE(1),
    0x08,
    LOGICAL_MINIMUM(1),
    0x00,
    LOGICAL_MAXIMUM(1),
    0x65,
    USAGE_MINIMUM(1),
    0x00,
    USAGE_MAXIMUM(1),
    0x65,
    INPUT(1),
    0x00, // Data, Array

    // LED output report
    REPORT_COUNT(1),
    0x05,
    REPORT_SIZE(1),
    0x01,
    USAGE_PAGE(1),
    0x08, // LEDs
    USAGE_MINIMUM(1),
    0x01,
    USAGE_MAXIMUM(1),
    0x05,
    OUTPUT(1),
    0x02, // Data, Variable, Absolute

    // LED padding
    REPORT_COUNT(1), 0x01, REPORT_SIZE(1), 0x03, OUTPUT(1), 0x01, // Constant
    END_COLLECTION(0)};
#endif