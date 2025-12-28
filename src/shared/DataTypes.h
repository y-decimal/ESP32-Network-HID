#ifndef DATATYPES_H
#define DATATYPES_H

#include <cstring>
#include <stdint.h>

struct Serializable {
  virtual size_t pack(uint8_t *output) const = 0;
  virtual size_t unpack(const uint8_t *input) = 0;
};

struct LocalBitMap : Serializable {
  const uint8_t *bitMapPtr;
  uint16_t size;

  size_t pack(uint8_t *output) const {

  }
};

#endif