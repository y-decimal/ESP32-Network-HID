#ifndef HIDMAPPER_H
#define HIDMAPPER_H

#include <cstdint>
#include <cstring>
#include <stdio.h>
#include <vector>
#include <unordered_map>

class HidMapper
{

private:
  uint8_t hidBitmap[32]{0}; // 32 Bytes = 256 bits for HID report

  std::unordered_map<uint8_t, std::vector<uint8_t>> localToHidMaps;

  void setBit(uint8_t bitmapBitIndex);
  void clearBit(uint8_t bitmapBitIndex);
  void updateHidBit(bool bitState, uint8_t bitmapBitIndex);

public:
  HidMapper();

  void insertMap(const uint8_t *map, size_t mapSize, uint8_t mapId);

  void mapBitmapToHidBitmap(const uint8_t *bitmap, size_t bitmapSize, uint8_t mapId);
  void mapIndexToHidBitmap(uint8_t index, bool bitState, uint8_t mapId);

  size_t getBitmapSize() { return 32; }
  size_t copyBitmap(uint8_t *dest, size_t destSize) const;
  bool doesMapExist(uint8_t mapId) const;
};

#endif