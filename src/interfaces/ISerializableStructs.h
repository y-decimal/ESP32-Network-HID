#ifndef ISERIALIZABLESTRUCT_H
#define ISERIALIZABLESTRUCT_H

#include <cstring>
#include <stdint.h>

struct Serializable {
  virtual size_t pack(uint8_t *output) const = 0;
  virtual size_t unpack(const uint8_t *input) = 0;
};

#endif