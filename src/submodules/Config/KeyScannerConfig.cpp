#include <submodules/Config/KeyScannerConfig.h>

void KeyScannerConfig::setPins(uint8_t *rowPinData, uint8_t rowSize,
                               uint8_t *colPinData, uint8_t colSize) {
  rowPins.assign(rowPinData, rowPinData + rowSize);
  rows = rowSize;
  colPins.assign(colPinData, colPinData + colSize);
  cols = colSize;
  bitMapSize = (rows * cols + 7) / 8;
}

void KeyScannerConfig::setRefreshRate(uint16_t rate) {
  if (rate < MIN_REFRESH_RATE || rate > MAX_REFRESH_RATE)
    return;
  refreshRate = rate;
}

void KeyScannerConfig::setBitmapSendFrequency(uint16_t frequency) {
  if (frequency < MIN_BITMAP_REFRESH_RATE ||
      frequency > MAX_BITMAP_REFRESH_RATE)
    // frequency is now the bitmap frequency in Hz
    // Limited to a range of 1-500 Hz to ensure reasonable bitmap rates
    return;
  bitMapSendFrequency = frequency;
}

void KeyScannerConfig::setConfig(KeyCfgParams config) {
  setPins(config.rowPins, config.rows, config.colPins, config.cols);
  setRefreshRate(config.refreshRate);
  setBitmapSendFrequency(config.bitMapSendInterval);
}

size_t KeyScannerConfig::packSerialized(uint8_t *output, size_t size) const {

  // Check if provided buffer is large enough
  size_t ownSize = getSerializedSize();
  if (size < ownSize)
    return 0;

  // Temporary buffer to hold serialized data
  uint8_t buffer[ownSize] = {};

  // Helper variables for serialization
  size_t index = 0;
  size_t totalWrite = 0;
  size_t objSize = 0;

  // Serialize rows
  objSize = sizeof(rows);
  memcpy(buffer, &rows, objSize);
  index += objSize;
  totalWrite += objSize;

  // Serialize cols
  objSize = sizeof(cols);
  memcpy(buffer + index, &cols, objSize);
  index += objSize;
  totalWrite += objSize;

  // Serialize bitMapSize
  objSize = sizeof(bitMapSize);
  memcpy(buffer + index, &bitMapSize, objSize);
  index += objSize;
  totalWrite += objSize;

  // Serialize rowPins
  objSize = rows;
  memcpy(buffer + index, rowPins.data(), objSize);
  index += objSize;
  totalWrite += objSize;

  // Serialize colPins
  objSize = cols;
  memcpy(buffer + index, colPins.data(), objSize);
  index += objSize;
  totalWrite += objSize;

  // Serialize refreshRate
  objSize = sizeof(refreshRate);
  memcpy(buffer + index, &refreshRate, objSize);
  index += objSize;
  totalWrite += objSize;

  // Serialize bitMapSendFrequency
  objSize = sizeof(bitMapSendFrequency);
  memcpy(buffer + index, &bitMapSendFrequency, objSize);
  totalWrite += objSize;

  // Copy serialized data to output buffer
  memcpy(output, buffer, totalWrite);

  return totalWrite;
}

size_t KeyScannerConfig::unpackSerialized(const uint8_t *input, size_t size) {
  // Don't check against getSerializedSize() since we don't know rows/cols yet
  // Just do basic size validation
  if (size < sizeof(rows) + sizeof(cols) + sizeof(bitMapSize))
    return 0;

  // Helper variables for deserialization
  size_t index = 0;
  size_t totalWrite = 0;
  size_t objSize = 0;

  // Deserialize rows
  objSize = sizeof(rows);
  memcpy(&rows, input, objSize);
  index += objSize;
  totalWrite += objSize;

  // Deserialize cols
  objSize = sizeof(cols);
  memcpy(&cols, input + index, objSize);
  index += objSize;
  totalWrite += objSize;

  // Deserialize bitMapSize
  objSize = sizeof(bitMapSize);
  memcpy(&bitMapSize, input + index, objSize);
  index += objSize;
  totalWrite += objSize;

  // Now validate the full size including pin data
  size_t expectedSize = sizeof(rows) + sizeof(cols) + sizeof(bitMapSize) +
                        rows + cols + sizeof(refreshRate) +
                        sizeof(bitMapSendFrequency);
  if (size < expectedSize)
    return 0;

  // Resize vectors BEFORE copying data into them
  rowPins.resize(rows);
  objSize = rows;
  memcpy(rowPins.data(), input + index, objSize);
  index += rows;
  totalWrite += objSize;

  // Resize vectors BEFORE copying data into them
  colPins.resize(cols);
  objSize = cols;
  memcpy(colPins.data(), input + index, objSize);
  index += cols;
  totalWrite += objSize;

  // Deserialize refreshRate
  objSize = sizeof(refreshRate);
  memcpy(&refreshRate, input + index, objSize);
  index += objSize;
  totalWrite += objSize;

  // Deserialize bitMapSendFrequency
  objSize = sizeof(bitMapSendFrequency);
  memcpy(&bitMapSendFrequency, input + index, objSize);
  totalWrite += objSize;

  return totalWrite;
}

size_t KeyScannerConfig::getSerializedSize() const {
  // Return the total size needed for serialization
  return sizeof(rows) + sizeof(cols) + sizeof(bitMapSize) + rows + cols +
         sizeof(refreshRate) + sizeof(bitMapSendFrequency);
}
