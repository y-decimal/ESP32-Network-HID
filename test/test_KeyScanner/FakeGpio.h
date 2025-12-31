#ifndef TEST_TEST_KEYSCANNER_FAKEGPIO_H_
#define TEST_TEST_KEYSCANNER_FAKEGPIO_H_

#include <interfaces/IGpio.h>
#include <unordered_map>

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