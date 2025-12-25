#include <KeyScanner.h>

KeyScanner::KeyScanner(uint8_t *bitMapPtr) {
  pinMode(COL_1_PIN, INPUT_PULLUP);
  pinMode(COL_2_PIN, INPUT_PULLUP);
  // Idle rows float high via pull-up to avoid sneaky current paths.
  pinMode(ROW_1_PIN, INPUT_PULLUP);
  pinMode(ROW_2_PIN, INPUT_PULLUP);
  keyState = bitMapPtr;
}


void KeyScanner::updateKeyState() {
  for (uint8_t row = 0; row < ROWS; row++) {
    // Set all rows to high-Z, then drive only the active row low.
    for (uint8_t pinIndex = 0; pinIndex < COLS; pinIndex++) {
      pinMode(rowPins[pinIndex], INPUT_PULLUP);
    }
    pinMode(rowPins[row], OUTPUT);
    digitalWrite(rowPins[row], LOW);
    delayMicroseconds(5); // settle time after driving row

    for (uint8_t col = 0; col < COLS; col++) {
      if (digitalRead(colPins[col]) == LOW)
        setKey(&row, &col);
      else
        clearKey(&row, &col);
    }
  }
}

void KeyScanner::setKey(uint8_t *row, uint8_t *col) {
  const uint8_t bitIndex = getBitIndex(row, col);
  keyState[bitIndex / 8] |= (1 << (bitIndex % 8));
}

void KeyScanner::clearKey(uint8_t *row, uint8_t *col) {
  const uint8_t bitIndex = getBitIndex(row, col);
  keyState[bitIndex / 8] &= ~(1 << (bitIndex % 8));
}

const bool KeyScanner::getKey(uint8_t *bitMap, uint8_t row, uint8_t col,
                              uint8_t cols) {
  const uint8_t bitIndex = getBitIndex(&row, &col);
  return bitMap[bitIndex / 8] & (1 << (bitIndex % 8));
};

const uint8_t KeyScanner::getBitIndex(uint8_t *row, uint8_t *col,
                                      uint8_t cols) {
  return *row * cols + *col;
}