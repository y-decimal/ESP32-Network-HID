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
  void pinMode(uint8_t pin, uint8_t mode) override {
    // do nothing
  }

  uint8_t digitalRead(uint8_t pin) override { return 1; }

  void digitalWrite(uint8_t pin, uint8_t value) override {
    // do nothing
  }
};

#endif // TEST_TEST_KEYSCANNER_FAKEGPIO_H_