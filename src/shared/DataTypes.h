#ifndef DATATYPES_H
#define DATATYPES_H

#include <cstring>
#include <stdint.h>

struct KeyScannerState {
  uint8_t *bitMap;
  uint8_t bitMapSize;
};

#endif