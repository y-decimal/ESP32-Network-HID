#ifndef GLOBALHELPERS_H
#define GLOBALHELPERS_H

#include <stdint.h>
#include <cstddef>
#include <type_traits>

template <typename T> static inline uint8_t calcCheckSum_8Bit(const T &data) {
  static_assert(std::is_trivially_copyable<T>::value,
                "Checksum only works on trivially copyable types");
  const uint8_t *bytes = reinterpret_cast<const uint8_t *>(&data);
  uint8_t crc = 0x00; // CRC-8 with polynomial 0x07, initial value 0x00
  for (size_t i = 0; i < sizeof(T); i++) {
    crc ^= bytes[i];
    for (uint8_t bit = 0; bit < 8; ++bit) {
      if (crc & 0x80) {
        crc = static_cast<uint8_t>((crc << 1) ^ 0x07);
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}

template <typename T> static inline uint16_t calcCheckSum_16Bit(const T &data) {
  static_assert(std::is_trivially_copyable<T>::value,
                "Checksum only works on trivially copyable types");
  const uint8_t *bytes = reinterpret_cast<const uint8_t *>(&data);
  uint16_t checksum = 0;
  for (size_t i = 0; i < sizeof(T); i++) {
    checksum += bytes[i];
  }
  return checksum;
}

#endif