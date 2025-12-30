#include <submodules/ConfigManager/KeyScannerConfig.h>

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
  size_t ownSize = getSerializedSize();
  if (size < ownSize)
    return 0;
  uint8_t buffer[ownSize] = {};

  size_t index = 0;
  size_t totalWrite = 0;
  size_t objSize = 0;

  objSize = sizeof(rows);
  memcpy(buffer, &rows, objSize);
  index += objSize;
  totalWrite += objSize;

  objSize = sizeof(cols);
  memcpy(buffer + index, &cols, objSize);
  index += objSize;
  totalWrite += objSize;

  objSize = sizeof(bitMapSize);
  memcpy(buffer + index, &bitMapSize, objSize);
  index += objSize;
  totalWrite += objSize;

  objSize = rows;
  memcpy(buffer + index, rowPins.data(), objSize);
  index += objSize;
  totalWrite += objSize;

  objSize = cols;
  memcpy(buffer + index, colPins.data(), objSize);
  index += objSize;
  totalWrite += objSize;

  objSize = sizeof(refreshRate);
  memcpy(buffer + index, &refreshRate, objSize);
  index += objSize;
  totalWrite += objSize;

  objSize = sizeof(bitMapSendFrequency);
  memcpy(buffer + index, &bitMapSendFrequency, objSize);
  totalWrite += objSize;

  memcpy(output, buffer, totalWrite);

  return totalWrite;
}

size_t KeyScannerConfig::unpackSerialized(const uint8_t *input, size_t size) {
  // Don't check against getSerializedSize() since we don't know rows/cols yet
  // Just do basic size validation
  if (size < sizeof(rows) + sizeof(cols) + sizeof(bitMapSize))
    return 0;

  size_t index = 0;
  size_t totalWrite = 0;
  size_t objSize = 0;

  objSize = sizeof(rows);
  memcpy(&rows, input, objSize);
  index += objSize;
  totalWrite += objSize;

  objSize = sizeof(cols);
  memcpy(&cols, input + index, objSize);
  index += objSize;
  totalWrite += objSize;

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

  objSize = sizeof(refreshRate);
  memcpy(&refreshRate, input + index, objSize);
  index += objSize;
  totalWrite += objSize;

  objSize = sizeof(bitMapSendFrequency);
  memcpy(&bitMapSendFrequency, input + index, objSize);
  totalWrite += objSize;

  return totalWrite;
}

size_t KeyScannerConfig::getSerializedSize() const {
  return sizeof(rows) + sizeof(cols) + sizeof(bitMapSize) + rows + cols +
         sizeof(refreshRate) + sizeof(bitMapSendFrequency);
}
