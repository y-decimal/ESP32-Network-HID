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

public:
  HidMapper();

  void insertMap(const uint8_t *map, size_t mapSize, uint8_t mapId);

  void mapToHidBitmap(const uint8_t *bitmap, size_t bitmapSize, uint8_t mapId);

  void updateHidBit(bool bitState, uint8_t bitmapPos);

  size_t copyBitmap(uint8_t *dest, size_t destSize) const;
};

#endif