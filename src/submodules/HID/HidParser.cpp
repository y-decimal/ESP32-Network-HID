#include <submodules/HID/HidParser.h>

HidParser::HidParser(const uint8_t *descriptor, size_t length)
{
    // Map descriptor to hidToOutMap and mapIterators here
}

void HidParser::mapBitmap(const uint8_t *hidMap, size_t hidMapSize)
{
    for (auto it : mapIterators)
    {
        for (uint8_t i = it.srcRangeStart; i < it.srcRangeStart + it.rangeLength; i++)
        {
            memcpy(outputBitmap + it.destRangeStart, hidMap + it.srcRangeStart, it.rangeLength);
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