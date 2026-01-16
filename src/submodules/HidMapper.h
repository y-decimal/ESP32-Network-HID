#ifndef HIDMAPPER_H
#define HIDMAPPER_H

#include <cstdint>
#include <cstring>
#include <stdio.h>

class HidMapper {

private:
  uint8_t hidBitmap[16]; // 16 Bytes = 128 bits for HID report
                         // Enough for our use case

public:
  HidMapper();

  void handleBitmapEvent(const uint8_t *bitmap, size_t bitmapSize, uint8_t *map,
                         size_t mapSize);

  size_t generateHidReport(uint8_t *out);
};

#endif