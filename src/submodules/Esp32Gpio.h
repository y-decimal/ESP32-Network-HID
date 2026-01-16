#ifndef ESP32GPIO_H
#define ESP32GPIO_H

#include <Arduino.h>
#include <interfaces/IGpio.h>

/*
 * @brief ESP32 GPIO implementation of the IGpio interface.
 *
 * This class provides an implementation of the IGpio interface
 * for ESP32 microcontrollers using the Arduino framework.
 * It maps the generic GPIO operations defined in IGpio
 * to the specific functions provided by the ESP32 Arduino core.
 */
class Esp32Gpio : public IGpio {
public:
  void pinMode(uint8_t pin, PinMode mode) override;

  PinState digitalRead(uint8_t pin) override;

  void digitalWrite(uint8_t pin, PinState value) override;
};

inline void Esp32Gpio::pinMode(uint8_t pin, PinMode mode) {
  switch (mode) {
  case PinMode::Input:
    ::pinMode(pin, INPUT);
    break;
  case PinMode::InputPullup:
    ::pinMode(pin, INPUT_PULLUP);
    break;
  case PinMode::Output:
    ::pinMode(pin, OUTPUT);
    break;
  }
}

inline PinState Esp32Gpio::digitalRead(uint8_t pin) {
  return static_cast<PinState>(::digitalRead(pin));
}

inline void Esp32Gpio::digitalWrite(uint8_t pin, PinState value) {
  ::digitalWrite(pin, static_cast<uint8_t>(value));
}

#endif // ESP32GPIO_H