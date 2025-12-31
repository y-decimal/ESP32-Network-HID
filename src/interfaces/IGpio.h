#ifndef IGPIO_H
#define IGPIO_H
#include <cstdint>

enum class PinMode : uint8_t { Input, InputPullup, Output };
enum class PinState : uint8_t { Low = 0, High = 1 };

class IGpio {
public:
  virtual void pinMode(uint8_t pin, PinMode mode) = 0;
  virtual void digitalWrite(uint8_t pin, PinState value) = 0;
  virtual PinState digitalRead(uint8_t pin) = 0;

  virtual ~IGpio() = default;
};
#endif