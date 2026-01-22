#include <submodules/HidMapper.h>

HidMapper::HidMapper() {};

void HidMapper::insertMap(const uint8_t *map, size_t mapSize, uint8_t mapId)
{
    std::vector<uint8_t> buffer;
    buffer.assign(map, map + mapSize);
    localToHidMaps[mapId] = buffer;
}

void HidMapper::mapToHidBitmap(const uint8_t *bitmap, size_t bitmapSize, uint8_t mapId)
{
    // Implement mapping a bitmap to the hidBitmap via map at localToHidMaps[map]
}

void HidMapper::updateHidBit(bool bitState, uint8_t bitmapBitIndex)
{
    bitState ? setBit(bitmapBitIndex) : clearBit(bitmapBitIndex);
}

size_t HidMapper::copyBitmap(uint8_t *dest, size_t destSize) const
{
    if (destSize < sizeof(hidBitmap))
        return 0;

    memcpy(dest, hidBitmap, sizeof(hidBitmap));
    return sizeof(hidBitmap);
}

void HidMapper::setBit(uint8_t bitmapBitIndex)
{
    uint8_t byteIndex = bitmapBitIndex / 8;
    uint8_t bitIndex = bitmapBitIndex % 8;
    hidBitmap[byteIndex] |= (1 << bitIndex);
}

void HidMapper::clearBit(uint8_t bitmapBitIndex)
{
    uint8_t byteIndex = bitmapBitIndex / 8;
    uint8_t bitIndex = bitmapBitIndex % 8;
    hidBitmap[byteIndex] &= ~(1 << bitIndex);
}