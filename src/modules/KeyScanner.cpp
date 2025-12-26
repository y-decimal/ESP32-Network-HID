#include <modules/KeyScanner.h>

void KeyScanner::updateKeyState() {
  memset(workingBuffer, 0, bitMapSize);

  for (uint8_t row = 0; row < rowCount; row++) {
    // Set all rows to high-Z, then drive only the active row low.
    for (uint8_t pinIndex = 0; pinIndex < rowCount; pinIndex++) {
      pinMode(rowPins[pinIndex], INPUT_PULLUP);
    }
    pinMode(rowPins[row], OUTPUT);
    digitalWrite(rowPins[row], LOW);
    // delayMicroseconds(5); // settle time after driving row

    for (uint8_t col = 0; col < colCount; col++) {
      bool isKeyPressed = (digitalRead(colPins[col]) == LOW);
      bool wasPressed = wasKeyPressed(row, col);
      if (isKeyPressed)
        setKey(row, col);
      if (isKeyPressed && !wasPressed && onKeyChange) {
        onKeyChange(getBitIndex(row, col), true); // Key press event detected
      } else if (!isKeyPressed && wasPressed && onKeyChange) {
        onKeyChange(getBitIndex(row, col), false); // Key release event detected
      }
    }
  }
  swapBuffers();
}

void KeyScanner::setKey(uint8_t row, uint8_t col) {
  uint16_t bitIndex = getBitIndex(row, col);
  workingBuffer[getByteIndex(row, col)] |= (getBitMask(row, col));
}

bool KeyScanner::wasKeyPressed(uint8_t row, uint8_t col) {
  return (publishedBuffer[getByteIndex(row, col)] & (getBitMask(row, col))) !=
         0;
}

inline void KeyScanner::swapBuffers() {
  std::swap(workingBuffer, publishedBuffer);
}

inline uint8_t KeyScanner::getBitMask(uint8_t row, uint8_t col) {
  return (1 << (getBitIndex(row, col) % 8));
}

inline uint16_t KeyScanner::getBitIndex(uint8_t row, uint8_t col) {
  return (row * colCount + col);
}

inline uint8_t KeyScanner::getByteIndex(uint8_t row, uint8_t col) {
  return getBitIndex(row, col) / 8;
}