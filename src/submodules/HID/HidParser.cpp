#include <submodules/HID/HidParser.h>
#include <HIDTypes.h>

HidParser::HidParser(const uint8_t *descriptor, size_t length) { buildMap(descriptor, length); }

void HidParser::mapBitmap(const uint8_t *hidMap, size_t hidMapSize)
{
    // Clear output bitmap first
    memset(outputBitmap, 0, outputSize);

    for (const Run &it : mapIterators)
    {
        // Copy bit by bit to handle all edge cases correctly
        for (uint8_t bitIdx = 0; bitIdx < it.rangeLength; bitIdx++)
        {
            uint16_t srcBitPos = it.srcRangeStart + bitIdx;
            uint16_t destBitPos = it.destRangeStart + bitIdx;

            uint8_t srcByteIdx = srcBitPos / 8;
            uint8_t srcBitInByte = srcBitPos % 8;

            uint8_t destByteIdx = destBitPos / 8;
            uint8_t destBitInByte = destBitPos % 8;

            // Extract bit from source
            uint8_t bitValue = (hidMap[srcByteIdx] >> srcBitInByte) & 1;

            // Set bit in destination
            if (bitValue)
                outputBitmap[destByteIdx] |= (1 << destBitInByte);

            // Update pressed keys vector
            updatePressedKeysVector(srcBitPos, bitValue);
        }
    }
}

size_t HidParser::getOutputSize()
{
    return outputSize;
}

void HidParser::getOutputBitmap(uint8_t *out, size_t outSize)
{
    if (outSize < outputSize)
        return;

    memcpy(out, outputBitmap, outputSize);
}

const std::vector<uint8_t> &HidParser::getPressedKeysInOrder() const
{
    return pressedKeysInOrder;
}

void HidParser::updatePressedKeysVector(uint8_t keyCode, bool pressedState)
{
    if (pressedState)
    {
        // Key is pressed - add if not already in vector
        for (uint8_t existingKey : pressedKeysInOrder)
        {
            if (existingKey == keyCode)
                return; // Already in vector, don't add duplicate
        }
        pressedKeysInOrder.push_back(keyCode);
    }
    else
    {
        // Key is not pressed - remove from vector if present
        for (int i = pressedKeysInOrder.size() - 1; i >= 0; i--)
        {
            if (pressedKeysInOrder[i] == keyCode)
            {
                pressedKeysInOrder.erase(pressedKeysInOrder.begin() + i);
                return; // Found and removed
            }
        }
    }
}

inline void HidParser::buildMap(const uint8_t *descriptor, size_t length)
{
    // HID Report Descriptor item type tags
    const uint8_t TYPE_MAIN = 0x00;
    const uint8_t TYPE_GLOBAL = 0x04;
    const uint8_t TYPE_LOCAL = 0x08;

    // Main item tags
    const uint8_t TAG_INPUT = 0x80;
    const uint8_t TAG_OUTPUT = 0x90;
    const uint8_t TAG_FEATURE = 0xB0;

    // Global item tags
    const uint8_t TAG_USAGE_PAGE = 0x04;
    const uint8_t TAG_LOGICAL_MIN = 0x14;
    const uint8_t TAG_LOGICAL_MAX = 0x24;
    const uint8_t TAG_REPORT_SIZE = 0x74;
    const uint8_t TAG_REPORT_COUNT = 0x94;
    const uint8_t TAG_REPORT_ID = 0x84;

    // Local item tags
    const uint8_t TAG_USAGE = 0x08;
    const uint8_t TAG_USAGE_MIN = 0x18;
    const uint8_t TAG_USAGE_MAX = 0x28;

    // Input/Output/Feature flags
    const uint8_t FLAG_CONSTANT = 0x01; // 0=Data, 1=Constant
    const uint8_t FLAG_VARIABLE = 0x02; // 0=Array, 1=Variable

    // Parsing state
    uint16_t usagePage = 0;
    uint16_t usageMin = 0;
    uint16_t usageMax = 0;
    uint16_t logicalMin = 0;
    uint16_t logicalMax = 0;
    uint8_t reportSize = 0;
    uint8_t reportCount = 0;
    bool hasUsageRange = false;

    size_t currentOutputBit = 0; // Track position in output bitmap

    mapIterators.clear();

    size_t i = 0;
    while (i < length)
    {
        uint8_t item = descriptor[i++];
        uint8_t itemType = item & 0x0C;
        uint8_t itemTag = item & 0xFC;
        uint8_t itemSize = item & 0x03;

        // Handle special case for 4-byte items
        if (itemSize == 3)
            itemSize = 4;

        // Extract item value
        uint32_t value = 0;
        for (uint8_t j = 0; j < itemSize && i < length; j++)
        {
            value |= ((uint32_t)descriptor[i++]) << (j * 8);
        }

        // Process item based on type and tag
        if (itemType == TYPE_GLOBAL)
        {
            switch (itemTag)
            {
            case TAG_USAGE_PAGE:
                usagePage = value;
                break;
            case TAG_LOGICAL_MIN:
                logicalMin = value;
                break;
            case TAG_LOGICAL_MAX:
                logicalMax = value;
                break;
            case TAG_REPORT_SIZE:
                reportSize = value;
                break;
            case TAG_REPORT_COUNT:
                reportCount = value;
                break;
            case TAG_REPORT_ID:
                // Skip report ID handling for now, but you may need it
                break;
            }
        }
        else if (itemType == TYPE_LOCAL)
        {
            switch (itemTag)
            {
            case TAG_USAGE_MIN:
                usageMin = value;
                hasUsageRange = true;
                break;
            case TAG_USAGE_MAX:
                usageMax = value;
                hasUsageRange = true;
                break;
            }
        }
        else if (itemType == TYPE_MAIN)
        {
            // Process Input items (we care about Input, not Output/Feature)
            if (itemTag == TAG_INPUT)
            {
                uint8_t flags = value & 0xFF;
                bool isConstant = flags & FLAG_CONSTANT;
                bool isVariable = flags & FLAG_VARIABLE;

                // Only process Data (non-Constant) items
                if (!isConstant)
                {
                    if (isVariable && hasUsageRange)
                    {
                        // Variable type with usage range (bitmap)
                        // Each bit represents one usage code
                        uint16_t rangeLength = usageMax - usageMin + 1;

                        // Ensure we're only processing the expected number of items
                        if (reportCount == rangeLength)
                        {
                            // Create a run mapping the usage range
                            Run run;
                            run.srcRangeStart = usageMin;
                            run.destRangeStart = currentOutputBit;
                            run.rangeLength = rangeLength;
                            mapIterators.push_back(run);

                            currentOutputBit += rangeLength;
                        }
                        else
                        {
                            // Report count doesn't match range, create individual mappings
                            for (uint8_t bit = 0; bit < reportCount; bit++)
                            {
                                Run run;
                                run.srcRangeStart = usageMin + bit;
                                run.destRangeStart = currentOutputBit++;
                                run.rangeLength = 1;
                                mapIterators.push_back(run);
                            }
                        }
                    }
                    else if (isVariable)
                    {
                        // Variable type without usage range
                        // Typically modifier keys or similar
                        for (uint8_t bit = 0; bit < reportCount; bit++)
                        {
                            Run run;
                            run.srcRangeStart = usageMin + bit;
                            run.destRangeStart = currentOutputBit++;
                            run.rangeLength = 1;
                            mapIterators.push_back(run);
                        }
                    }
                    // Note: Array type (non-Variable) items contain HID codes directly,
                    // not a bitmap, so we skip those for bitmap-based parsing
                }

                // Reset local items after Main item
                usageMin = 0;
                usageMax = 0;
                hasUsageRange = false;
            }
        }
    }

    // Calculate output bitmap size in bytes
    outputSize = (currentOutputBit + 7) / 8;

    // Allocate output bitmap
    if (outputBitmap != nullptr)
        delete[] outputBitmap;
    outputBitmap = new uint8_t[outputSize]();
}