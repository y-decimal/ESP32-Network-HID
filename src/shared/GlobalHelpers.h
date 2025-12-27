#ifndef GLOBALHELPERS_H
#define GLOBALHELPERS_H

#include <stdint.h>

template <typename T> static inline uint8_t calcCheckSum_8Bit(const T &data) {
  static_assert(std::is_trivially_copyable<T>::value,
                "Checksum only works on trivially copyable types");
  const uint8_t *bytes = reinterpret_cast<const uint8_t *>(data);
  uint8_t checksum = 0;
  for (size_t i = 0; i < sizeof(T); i++) {
    checksum ^= bytes[i];
  }
  return checksum;
}

template <typename T> static inline uint16_t calcCheckSum_16Bit(const T &data) {
  static_assert(std::is_trivially_copyable<T>::value,
                "Checksum only works on trivially copyable types");
  const uint8_t *bytes = reinterpret_cast<const uint8_t *>(data);
  uint16_t checksum = 0;
  for (size_t i = 0; i < sizeof(T); i++) {
    checksum += bytes[i];
  }
  return checksum;
}

#endif