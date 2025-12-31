#ifndef TEST_TEST_KEYSCANNER_FAKEGPIO_H_
#define TEST_TEST_KEYSCANNER_FAKEGPIO_H_

#include <interfaces/IGpio.h>
#include <unordered_map>

#define INPUT_PULLUP 0x2
#define INPUT 0x0
#define OUTPUT 0x1
#define LOW 0x0
#define HIGH 0x1

class FakeGpio : public IGpio {
public:
  void pinMode(uint8_t pin, PinMode mode) override {
    // do nothing
  }

  PinState digitalRead(uint8_t pin) override { return PinState::High; }

  void digitalWrite(uint8_t pin, PinState value) override {
    // do nothing
  }
};

#endif // TEST_TEST_KEYSCANNER_FAKEGPIO_H_