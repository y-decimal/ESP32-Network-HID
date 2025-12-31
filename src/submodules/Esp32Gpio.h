#ifndef ESP32GPIO_H
#define ESP32GPIO_H

#include <Arduino.h>
#include <interfaces/IGpio.h>

class Esp32Gpio : public IGpio {
public:
  void pinMode(uint8_t pin, uint8_t mode) override;

  uint8_t digitalRead(uint8_t pin) override;

  void digitalWrite(uint8_t pin, uint8_t value) override;
};

inline void Esp32Gpio::pinMode(uint8_t pin, uint8_t mode) {
  ::pinMode(pin, mode);
}

inline uint8_t Esp32Gpio::digitalRead(uint8_t pin) {
  return ::digitalRead(pin);
}

inline void Esp32Gpio::digitalWrite(uint8_t pin, uint8_t value) {
  ::digitalWrite(pin, value);
}

#endif // ESP32GPIO_H