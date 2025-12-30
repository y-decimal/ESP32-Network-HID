#ifndef ISERIALIZABLESTRUCT_H
#define ISERIALIZABLESTRUCT_H

#include <cstring>
#include <stdint.h>

class Serializable {
public:
  virtual ~Serializable() = default;
  virtual size_t packSerialized(uint8_t *output, size_t size) const = 0;
  virtual size_t unpackSerialized(const uint8_t *input, size_t size) = 0;
  virtual size_t getSerializedSize() const = 0;
};

#endif