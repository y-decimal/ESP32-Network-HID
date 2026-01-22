#ifndef KEYSCANNER_H
#define KEYSCANNER_H

#include <cstdint>
#include <cstring>
#include <functional>
#include <interfaces/IGpio.h>
#include <vector>

/**
 * @brief KeyScanner class for scanning a matrix of keys.
 *
 * The KeyScanner class manages the scanning of a key matrix
 * connected via GPIO pins.
 *
 * It detects key presses and releases,
 * maintains the current state of the keys, and allows registration
 * of callback functions to handle key state changes.
 */
class KeyScanner
{
private:
  // Reference to the GPIO interface for pin operations. Interface allows for
  // hardware independent implementation.
  IGpio &gpio;

  // Configuration for row and column pins and their counts.
  const uint8_t *rowPins;
  const uint8_t *colPins;
  size_t rowCount;
  size_t colCount;

  // Size of the bitmap representing key states in bytes.
  size_t bitmapSize;

  // Buffers for storing key states. Used for double buffering to avoid
  // read/write conflicts.
  std::vector<uint8_t> keyMapSwapBufferA;
  std::vector<uint8_t> keyMapSwapBufferB;

  // Pointers to the current working and published buffers.
  uint8_t *workingBuffer;
  uint8_t *publishedBuffer;

  // Callback function to be invoked on key state changes.
  std::function<void(uint16_t keyIndex, bool pressed)> onKeyChange;

  // Internal helper methods
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
  /**
   * @brief Constructor for KeyScanner.
   * @param gpio Reference to the IGpio interface for GPIO operations.
   * @param rowPins Array of GPIO pin numbers for the rows.
   * @param colPins Array of GPIO pin numbers for the columns.
   * @param rowCount Number of rows in the key matrix.
   * @param colCount Number of columns in the key matrix.
   */
  KeyScanner(IGpio &gpio, const uint8_t *rowPins, const uint8_t *colPins,
             const uint8_t rowCount, const uint8_t colCount);

  /**
   * @brief Registers a callback function to be invoked on key state changes.
   * @param callback The callback function taking keyIndex and pressed state.
   */
  void registerOnKeyChangeCallback(
      const std::function<void(uint16_t keyIndex, bool pressed)> &callback)
  {
    onKeyChange = callback;
  }

  /**
   * @brief Clears the registered key change callback.
   */
  void clearOnKeyChangeCallback() { onKeyChange = nullptr; }

  /**
   * @brief Copies the published key state bitmap to the provided destination
   * buffer.
   * @param dest Pointer to the destination buffer.
   * @param destSize Size of the destination buffer in bytes.
   */
  void copyPublishedBitmap(uint8_t *dest, size_t destSize) const;

  /**
   * @brief Gets the size of the key state bitmap in bytes.
   * @return Size of the bitmap in bytes.
   */
  const size_t getBitMapSize() const { return bitmapSize; }

  /**
   * @brief Scans the key matrix and updates key states.
   *
   * This method should be called periodically to detect key state changes.
   */
  void updateKeyState();
};

#endif