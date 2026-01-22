#include <submodules/HidMapper.h>
#include <submodules/Logger.h>
static Logger log(__FILENAME__);

HidMapper::HidMapper() {};

void HidMapper::insertMap(const uint8_t *map, size_t mapSize, uint8_t mapId)
{
    log.debug("Inserted map of size %d with ID %d", mapSize, mapId);
    std::vector<uint8_t> buffer;
    buffer.assign(map, map + mapSize);
    localToHidMaps[mapId] = buffer;
}

void HidMapper::mapBitmapToHidBitmap(const uint8_t *bitmap, size_t bitmapSize, uint8_t mapId)
{
    // Implement mapping a bitmap to the hidBitmap via map at localToHidMaps[map]
}

void HidMapper::mapIndexToHidBitmap(uint8_t index, bool bitState, uint8_t mapId)
{
    if (!doesMapExist(mapId))
    {
        log.warn("Could not map Index, no map found");
        return;
    }

    if (index > localToHidMaps[mapId].size())
    {
        log.warn("Could not map Index, index not inside map");
        return;
    }

    uint8_t mappedBitmapIndex = localToHidMaps[mapId][index];
    updateHidBit(bitState, mappedBitmapIndex);
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

bool HidMapper::doesMapExist(uint8_t mapId) const
{
    return localToHidMaps.find(mapId) != localToHidMaps.end();
}