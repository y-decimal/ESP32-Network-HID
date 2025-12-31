#ifndef KEYSCANNER_H
#define KEYSCANNER_H

#include <functional>
#include <interfaces/IGpio.h>
#include <vector>
#include <cstdint>
#include <cstring>

class KeyScanner {
private:
  IGpio &gpio;

  const uint8_t *rowPins;
  const uint8_t *colPins;
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
  KeyScanner(IGpio &gpio, const uint8_t *rowPins, const uint8_t *colPins,
             const uint8_t rowCount, const uint8_t colCount);

  void registerOnKeyChangeCallback(
      const std::function<void(uint16_t keyIndex, bool pressed)> &callback) {
    onKeyChange = callback;
  }

  void clearOnKeyChangeCallback() { onKeyChange = nullptr; }

  void copyPublishedBitmap(uint8_t *dest, size_t destSize) const;

  const size_t getBitMapSize() const { return bitMapSize; }

  void updateKeyState();
};

#endif