#ifndef IGPIO_H
#define IGPIO_H
#include <cstdint>

class IGpio {
public:
  virtual void pinMode(uint8_t pin, uint8_t mode) = 0;
  virtual void digitalWrite(uint8_t pin, uint8_t value) = 0;
  virtual uint8_t digitalRead(uint8_t pin) = 0;

  virtual ~IGpio() = default;
};
#endif