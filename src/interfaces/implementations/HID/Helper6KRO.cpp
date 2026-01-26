#include <interfaces/implementations/HID/Helper6KRO.h>

#include <submodules/Logger.h>

static Logger log(SixKroHelper::NAMESPACE);

void SixKroHelper::convertBitmapTo6KRO(const uint8_t *bitmap, size_t bitmapSize, uint8_t *outputReport)
{
    if (lastBitmap == nullptr)
    {
        lastBitmap = (uint8_t *)calloc(bitmapSize, 1);
        log.debug("Initialized lastBitmap with size %zu", bitmapSize);
    }

    uint8_t modifierByte = 0;

    for (uint16_t hidCode = 0; hidCode < bitmapSize * 8; hidCode++)
    {
        bool wasDown = lastBitmap[hidCode / 8] & (1 << (hidCode % 8));
        bool isDown = bitmap[hidCode / 8] & (1 << (hidCode % 8));

        if (!wasDown && isDown)
        {
            log.debug("Key 0x%02X pressed", hidCode);
            if (isModifier(hidCode))
                modifierByte |= (1 << (hidCode - 0xE0)); // Convert HID code to bit position
            else
                pressedKeysInOrder.push_back(hidCode);
        }
        else if (wasDown && !isDown)
        {
            log.debug("Key 0x%02X released", hidCode);
            if (isModifier(hidCode))
                modifierByte &= ~(1 << (hidCode - 0xE0)); // Convert HID code to bit position and clear
            else
                for (int i = 0; i < pressedKeysInOrder.size(); i++)
                {
                    if (pressedKeysInOrder[i] == hidCode)
                        pressedKeysInOrder.erase(pressedKeysInOrder.begin() + i);
                }
        }
    }

    // Update lastBitmap AFTER processing changes
    memcpy(lastBitmap, bitmap, bitmapSize);

    memcpy(outputReport, &modifierByte, 1);
    memcpy(outputReport + 1, "\x00", 1); // Reserved byte

    size_t count = 0;
    for (uint8_t hidCode : pressedKeysInOrder)
    {
        if (isModifier(hidCode))
        {
            log.debug("Skipping modifier key 0x%02X in key array", hidCode);
            continue;
        }
        if (count >= 6)
        {
            log.warn("More than 6 keys pressed, ignoring additional keys");
            break;
        }
        memcpy(outputReport + 2 + count, &hidCode, 1);
        log.debug("Added key 0x%02X to output report at position %zu", hidCode, count);
        count++;
    }
    log.debug("Output report prepared with %zu keys pressed", count);
}

bool SixKroHelper::isModifier(uint8_t hidCode)
{
    switch (hidCode)
    {
    case KEY_MOD_LCTRL:
    case KEY_MOD_LSHIFT:
    case KEY_MOD_LALT:
    case KEY_MOD_LMETA:
    case KEY_MOD_RCTRL:
    case KEY_MOD_RSHIFT:
    case KEY_MOD_RALT:
    case KEY_MOD_RMETA:
        return true;
    default:
        return false;
    }
}

uint8_t SixKroHelper::reportDescriptor6KRO[63] = {
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
    HIDINPUT(1),
    0x02, // Data, Variable, Absolute

    // Reserved byte
    REPORT_COUNT(1),
    0x01,
    REPORT_SIZE(1),
    0x08,
    HIDINPUT(1),
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
    HIDINPUT(1),
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
    HIDOUTPUT(1),
    0x02, // Data, Variable, Absolute

    // LED padding
    REPORT_COUNT(1), 0x01, REPORT_SIZE(1), 0x03, HIDOUTPUT(1), 0x01, // Constant
    END_COLLECTION(0)};
