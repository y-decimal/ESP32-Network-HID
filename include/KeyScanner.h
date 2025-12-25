#ifndef KEYSCANNER_H
#define KEYSCANNER_H

#include <Arduino.h>

// For now we will assume the pins and wiring layout are always the same for
// simplicity, may be made more modular later

#define ROW_1_PIN 9
#define ROW_2_PIN 10
#define COL_1_PIN 17
#define COL_2_PIN 18
#define ROWS 2
#define COLS 2

static constexpr int rowPins[ROWS] = {ROW_1_PIN, ROW_2_PIN};
static constexpr int colPins[COLS] = {COL_1_PIN, COL_2_PIN};

class KeyScanner {
private:
  uint8_t keyState[5] = {0};

  void updateKeyState();
  void setKey(uint8_t *row, uint8_t *col);
  void clearKey(uint8_t *row, uint8_t *col);
  const uint8_t getBitIndex(uint8_t *row, uint8_t *col, uint8_t cols = COLS);

public:
  KeyScanner();
  const uint8_t *getKeyState();
  const bool getKey(uint8_t *bitMap, uint8_t row, uint8_t col, uint8_t cols);
};

#endif