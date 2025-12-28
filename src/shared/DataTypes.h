#ifndef DATATYPES_H
#define DATATYPES_H

#include <stdint.h>
#include <cstring>

struct KeyScannerState {
  uint8_t *bitMap;
  size_t bitMapSize;
};

#endif