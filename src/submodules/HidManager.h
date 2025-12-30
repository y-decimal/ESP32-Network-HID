#ifndef HIDMANAGER_H
#define HIDMANAGER_H

#include <cstdint>
#include <cstring>
#include <stdio.h>

class HidManager {

private:
  uint8_t hidBitmap[16]; // 16 Bytes = 128 bits for HID report
                         // Enough for our use case

  size_t generateHidReport(uint8_t *out);

public:
  HidManager();

  void handleBitmapEvent(const uint8_t *bitmap, size_t size, uint8_t *map);
};

#endif