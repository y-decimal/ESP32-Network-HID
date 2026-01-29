#ifndef HIDPARSER_H
#define HIDPARSER_H

#include <stdint.h>
#include <cstring>
#include <vector>

/**
 * @brief This class reads the unified, continous bitmap the system produces, then selectively maps it to
 * a continous, ordered output bitmap. It takes an HID descriptor as its input, uses it to parse only the
 * specific HID codes we care about, as well as the order in which they appear in the output, and uses
 * this information to produce the continous output bitmap, ordered by HID descriptor appearance.
 * It does so efficiently by only iterating over the bits it actually needs, skipping all bits the output
 * does not care about
 */
class HidParser
{
public:
    /**
     * @brief Constructs the HidParser
     * @param descriptor The pointer to the HID descriptor, must only be valid during constructor call
     * @param length The length of the descriptor in bytes
     */
    HidParser(const uint8_t *descriptor, size_t length);

    /**
     * @brief Maps the HID bitmap to the selective, ordered output Bitmap
     * @param hidMap Const pointer to the hidMap
     * @param hidMapSize Size of the hidMap in bytes
     */
    void mapBitmap(const uint8_t *hidMap, size_t hidMapSize);

    /**
     * @brief Get the size of the output bitmap in bytes
     * @return Size of outputBitmap in bytes
     */
    size_t getOutputSize();

    /**
     * @brief Copy the output bitmap to an external buffer
     * @param out The external buffer to copy into
     * @param outSize The size of the external buffer in bytes, must be at least the same as
     * the size of the output bitmap, use getOutputSize() if unsure
     */
    void getOutputBitmap(uint8_t *out, size_t outSize);

private:
    struct Run // All Ranges and lengths are in BIT, not Byte
    {
        uint8_t srcRangeStart;
        uint8_t destRangeStart;
        uint8_t rangeLength;
    };

    std::vector<Run> mapIterators;

    uint8_t *outputBitmap;
    size_t outputSize = 0;

    void buildMap(const uint8_t *descriptor, size_t length);
};

#endif