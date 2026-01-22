#include <submodules/KeyScanner.h>

KeyScanner::KeyScanner(IGpio &gpio, const uint8_t *rowPins,
                       const uint8_t *colPins, const uint8_t rowCount,
                       const uint8_t colCount)
    : gpio(gpio), rowPins(rowPins), colPins(colPins), rowCount(rowCount),
      colCount(colCount)
{

  // Calculate bitmap size in bytes
  bitmapSize = (rowCount * colCount + 7) / 8;

  // Initialize double buffers and size them appropriately
  keyMapSwapBufferA.resize(bitmapSize);
  keyMapSwapBufferB.resize(bitmapSize);

  // Set initial buffer pointers
  workingBuffer = keyMapSwapBufferA.data();
  publishedBuffer = keyMapSwapBufferB.data();

  // Initialize GPIO pins as input pull-ups
  for (size_t r = 0; r < rowCount; r++)
  {
    gpio.pinMode(rowPins[r], PinMode::InputPullup);
  }
  for (size_t c = 0; c < colCount; c++)
  {
    gpio.pinMode(colPins[c], PinMode::InputPullup);
  }
}

void KeyScanner::updateKeyState()
{
  // Clear the working buffer for fresh scan
  memset(workingBuffer, 0, bitmapSize);

  // Scan each row
  for (uint8_t row = 0; row < rowCount; row++)
  {
    // Set all rows to high-Z, then drive only the active row low.
    for (uint8_t pinIndex = 0; pinIndex < rowCount; pinIndex++)
    {
      gpio.pinMode(rowPins[pinIndex], PinMode::InputPullup);
    }
    gpio.pinMode(rowPins[row], PinMode::Output);
    gpio.digitalWrite(rowPins[row], PinState::Low);

    // Scan each column for key presses
    for (uint8_t col = 0; col < colCount; col++)
    {
      bool isKeyPressed = (gpio.digitalRead(colPins[col]) == PinState::Low);
      bool wasPressed = wasKeyPressed(row, col);

      // Update working buffer and trigger callbacks on state change
      if (isKeyPressed)
        setKey(row, col);
      if (isKeyPressed && !wasPressed && onKeyChange)
      {
        onKeyChange(getBitIndex(row, col), true); // Key press event detected
      }
      else if (!isKeyPressed && wasPressed && onKeyChange)
      {
        onKeyChange(getBitIndex(row, col), false); // Key release event detected
      }
    }
  }
  // Swap the working and published buffers
  swapBuffers();
}

void KeyScanner::setKey(uint8_t row, uint8_t col)
{
  // Set the corresponding bit in the working buffer
  workingBuffer[getByteIndex(row, col)] |= (getBitMask(row, col));
}

bool KeyScanner::wasKeyPressed(uint8_t row, uint8_t col)
{
  // Check the corresponding bit in the published buffer
  return (publishedBuffer[getByteIndex(row, col)] & (getBitMask(row, col))) !=
         0;
}

inline void KeyScanner::swapBuffers()
{
  // Swap the working and published buffer pointers
  std::swap(workingBuffer, publishedBuffer);
}

inline uint8_t KeyScanner::getBitMask(uint8_t row, uint8_t col)
{
  // Return the bitmask for the specific key position
  return (1 << (getBitIndex(row, col) % 8));
}

inline uint16_t KeyScanner::getBitIndex(uint8_t row, uint8_t col)
{
  // Calculate the linear bit index for the key at (row, col)
  return (row * colCount + col);
}

inline uint8_t KeyScanner::getByteIndex(uint8_t row, uint8_t col)
{
  // Calculate the byte index in the buffer for the key at (row, col)
  return getBitIndex(row, col) / 8;
}

void KeyScanner::copyPublishedBitmap(uint8_t *dest, size_t destSize) const
{
  // Copy the published key state bitmap to the provided destination buffer
  size_t n = std::min(bitmapSize, destSize);
  memcpy(dest, publishedBuffer, n);
  if (destSize > n)
  {
    memset(dest + n, 0, destSize - n);
  }
}