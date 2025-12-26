#include <modules/KeyScanner.h>

template <size_t Rows, size_t Cols>
KeyScanner::KeyScanner(const uint8_t (&rowPins)[Rows],
                       const uint8_t (&colPins)[Cols]) {

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
      if (digitalRead(colPins[col]) == LOW)
        setKey(row, col);
    }
  }

  uint8_t *old = publishedBuffer;
  publishedBuffer = workingBuffer;
  workingBuffer = old;
}

void KeyScanner::setKey(uint8_t row, uint8_t col) {
  uint16_t bitIndex = getBitIndex(row, col, colCount);
  workingBuffer[bitIndex / 8] |= (1 << (bitIndex % 8));
}

void KeyScanner::clearKey(uint8_t row, uint8_t col) {
  uint16_t bitIndex = getBitIndex(row, col, colCount);
  workingBuffer[bitIndex / 8] &= ~(1 << (bitIndex % 8));
}

inline uint16_t KeyScanner::getBitIndex(uint8_t row, uint8_t col, size_t cols) {
  return row * cols + col;
}