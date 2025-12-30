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

void KeyScannerConfig::setBitMapSendInterval(uint16_t rateDivisor) {
  if (MIN_BITMAP_INTERVAL < 2 || rateDivisor > MAX_BITMAP_INTERVAL)
    // Limited to a range of 2-5000, to ensure bitmaps aren't sent every
    // single loop (as this would most likely block the ESP communication),
    // and 5000 to ensure we don't exceed the uint16_t limit and also stay
    // within reasonable bounds for regular bitMap refreshes
    return;
  bitMapSendInterval = rateDivisor;
}

void KeyScannerConfig::setConfig(KeyCfgParams config) {
  setPins(config.rowPins, config.rows, config.colPins, config.cols);
  setRefreshRate(config.refreshRate);
  setBitMapSendInterval(config.bitMapSendInterval);
}

size_t KeyScannerConfig::packSerialized(uint8_t *output, size_t size) const {
  size_t ownSize = getSerializedSize();
  if (size < ownSize)
    return 0;
  uint8_t buffer[ownSize] = {};

  size_t index = 0;
  memcpy(buffer, &rows, sizeof(rows));
  index += sizeof(rows);

  memcpy(buffer + index, &cols, sizeof(cols));
  index += sizeof(cols);

  memcpy(buffer + index, &bitMapSize, sizeof(bitMapSize));
  index += sizeof(bitMapSize);

  memcpy(buffer + index, rowPins.data(), rows);
  index += rows;

  memcpy(buffer + index, colPins.data(), cols);
  index += cols;

  memcpy(buffer + index, &refreshRate, sizeof(refreshRate));
  index += sizeof(refreshRate);

  memccpy(buffer + index, &bitMapSendInterval, sizeof(bitMapSendInterval));

  memcpy(output, buffer, size);

  return size;
}

size_t KeyScannerConfig::unpackSerialized(const uint8_t *input, size_t size) {
  size_t ownSize = getSerializedSize();
  if (size > ownSize)
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

  objSize = rows;
  memcpy(rowPins.data(), input + index, objSize);
  index += rows;
  totalWrite += objSize;

  objSize = cols;
  memcpy(colPins.data(), input + index, objSize);
  index += cols;
  totalWrite += objSize;

  objSize = sizeof(refreshRate);
  memcpy(&refreshRate, input + index, objSize);
  index += objSize;
  totalWrite += objSize;

  objSize = sizeof(bitMapSendInterval);
  memcpy(&bitMapSendInterval, input + index, objSize);
  totalWrite += objSize;

  return totalWrite;
}

size_t KeyScannerConfig::getSerializedSize() const {
  return sizeof(rows) + sizeof(cols) + sizeof(bitMapSize) + rows + cols +
         sizeof(refreshRate) + sizeof(bitMapSendInterval);
}
