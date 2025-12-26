#ifndef KEYSCANNER_H
#define KEYSCANNER_H

#include <Arduino.h>
#include <vector>

class KeyScanner {
private:
  std::vector<uint8_t> rowPins;
  std::vector<uint8_t> colPins;
  size_t rowCount;
  size_t colCount;

  size_t bitMapSize;

  std::vector<uint8_t> keyMapSwapBufferA;
  std::vector<uint8_t> keyMapSwapBufferB;

  uint8_t *workingBuffer;
  uint8_t *publishedBuffer;

  std::function<void(uint16_t keyIndex, bool pressed)> onKeyChange;

  void setKey(uint8_t row, uint8_t col);
  bool wasKeyPressed(uint8_t row, uint8_t col);
  uint16_t getBitIndex(uint8_t row, uint8_t col);

#ifdef UNIT_TEST
  friend class TestKeyScanner;
#endif

public:
  template <size_t Rows, size_t Cols>
  KeyScanner(const uint8_t (&rowPins)[Rows], const uint8_t (&colPins)[Cols]);
  void updateKeyState();
  const uint8_t *getBitMap() const { return publishedBuffer; }
  const size_t getBitMapSize() const { return bitMapSize; }
};

#endif