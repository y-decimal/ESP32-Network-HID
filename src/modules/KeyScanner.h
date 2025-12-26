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
  void swapBuffers();
  uint8_t getBitMask(uint8_t row, uint8_t col);
  uint16_t getBitIndex(uint8_t row, uint8_t col);
  uint8_t getByteIndex(uint8_t row, uint8_t col);

#ifdef UNIT_TEST
  friend class TestKeyScanner;
#endif

public:
  template <size_t Rows, size_t Cols>
  KeyScanner(const uint8_t (&rowPins)[Rows], const uint8_t (&colPins)[Cols]) {
    rowCount = Rows;
    colCount = Cols;

    this->rowPins.assign(rowPins, rowPins + Rows);
    this->colPins.assign(colPins, colPins + Cols);

    bitMapSize = (rowCount * colCount + 7) / 8;

    keyMapSwapBufferA.resize(bitMapSize);
    keyMapSwapBufferB.resize(bitMapSize);

    workingBuffer = keyMapSwapBufferA.data();
    publishedBuffer = keyMapSwapBufferB.data();

    for (size_t r = 0; r < rowCount; r++) {
      pinMode(rowPins[r], INPUT_PULLUP);
    }
    for (size_t c = 0; c < colCount; c++) {
      pinMode(colPins[c], INPUT_PULLUP);
    }
  }

  void registerOnKeyChangeCallback(
      const std::function<void(uint16_t keyIndex, bool pressed)> &callback) {
    onKeyChange = callback;
  }

  void clearOnKeyChangeCallback() { onKeyChange = nullptr; }

  void copyPublishedBitmap(uint8_t *dest) const {
    memcpy(dest, publishedBuffer, bitMapSize);
  }

  const size_t getBitMapSize() const { return bitMapSize; }

  void updateKeyState();
};

#endif