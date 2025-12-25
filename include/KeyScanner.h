#ifndef KEYSCANNER_H
#define KEYSCANNER_H

#include <Arduino.h>

class KeyScanner {
private:
  uint8_t *keyState;
  static uint8_t rows;
  static uint8_t cols;
  uint8_t *rowPins;
  uint8_t *colPins;
  void setKey(uint8_t *row, uint8_t *col);
  void clearKey(uint8_t *row, uint8_t *col);
  const uint8_t getBitIndex(uint8_t *row, uint8_t *col, uint8_t *cols);

#ifdef UNIT_TEST
  friend class TestKeyScanner;
#endif

public:
  KeyScanner(uint8_t *bitMapPtr, uint8_t *rowPins, uint8_t *colPins);
  void updateKeyState();
  const bool getKey(uint8_t *bitMap, uint8_t row, uint8_t col, uint8_t cols);
};

#endif