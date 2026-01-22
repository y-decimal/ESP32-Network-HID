#include <submodules/Config/KeyScannerConfig.h>
#include <submodules/Logger.h>

static Logger log("KeyScannerConfig");

void KeyScannerConfig::setPins(uint8_t *rowPinData, uint8_t rowSize,
                               uint8_t *colPinData, uint8_t colSize)
{
  rowPins.assign(rowPinData, rowPinData + rowSize);
  rowCount = rowSize;
  colPins.assign(colPinData, colPinData + colSize);
  colCount = colSize;
  bitmapSize = (rowCount * colCount + 7) / 8;
}

void KeyScannerConfig::setRefreshRate(uint16_t rate)
{
  if (rate < MIN_REFRESH_RATE || rate > MAX_REFRESH_RATE)
    return;
  refreshRate = rate;
}

void KeyScannerConfig::setBitmapSendFrequency(uint16_t frequency)
{
  if (frequency < MIN_BITMAP_REFRESH_RATE ||
      frequency > MAX_BITMAP_REFRESH_RATE) {
    // frequency is now the bitmap frequency in Hz
    // Limited to a range of 1-500 Hz to ensure reasonable bitmap rates
    log.warn("Bitmap send frequency %d Hz is out of bounds (%d-%d Hz)",
             frequency, MIN_BITMAP_REFRESH_RATE, MAX_BITMAP_REFRESH_RATE);
    return;
  }
  bitMapSendRate = frequency;
}

void KeyScannerConfig::setLocalToHidMap(uint8_t *mapData, size_t mapSize)
{
  if (mapSize > MAX_KEY_COUNT) {
    log.warn("Local to HID map size %d exceeds maximum of %d", mapSize, MAX_KEY_COUNT);
    return;
  }
  localToHidMap.assign(mapData, mapData + mapSize);
}

void KeyScannerConfig::updateHIDCodeForIndex(uint8_t localKeyIndex, uint8_t hidCode)
{
  if (localKeyIndex >= localToHidMap.size()) {
    log.warn("Attempted to update HID code for out-of-bounds index %d", localKeyIndex);
    return;
  }
  localToHidMap[localKeyIndex] = hidCode;
}

void KeyScannerConfig::setConfig(KeyCfgParams config)
{
  setPins(config.rowPins, config.rowCount, config.colPins, config.colCount);
  setRefreshRate(config.refreshRate);
  setBitmapSendFrequency(config.bitmapSendRate);
  setLocalToHidMap(config.localToHidMap, (config.rowCount * config.colCount));
}

size_t KeyScannerConfig::packSerialized(uint8_t *output, size_t size) const
{

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

  // Serialize rowCount
  objSize = sizeof(rowCount);
  memcpy(buffer, &rowCount, objSize);
  index += objSize;
  totalWrite += objSize;

  // Serialize colCount
  objSize = sizeof(colCount);
  memcpy(buffer + index, &colCount, objSize);
  index += objSize;
  totalWrite += objSize;

  // Serialize bitmapSize
  objSize = sizeof(bitmapSize);
  memcpy(buffer + index, &bitmapSize, objSize);
  index += objSize;
  totalWrite += objSize;

  // Serialize rowPins
  objSize = rowCount;
  memcpy(buffer + index, rowPins.data(), objSize);
  index += objSize;
  totalWrite += objSize;

  // Serialize colPins
  objSize = colCount;
  memcpy(buffer + index, colPins.data(), objSize);
  index += objSize;
  totalWrite += objSize;

  // Serialize refreshRate
  objSize = sizeof(refreshRate);
  memcpy(buffer + index, &refreshRate, objSize);
  index += objSize;
  totalWrite += objSize;

  // Serialize bitMapSendFrequency
  objSize = sizeof(bitMapSendRate);
  memcpy(buffer + index, &bitMapSendRate, objSize);
  totalWrite += objSize;

  // Copy serialized data to output buffer
  memcpy(output, buffer, totalWrite);

  return totalWrite;
}

size_t KeyScannerConfig::unpackSerialized(const uint8_t *input, size_t size)
{
  // Don't check against getSerializedSize() since we don't know rowCount/colCount yet
  // Just do basic size validation
  if (size < sizeof(rowCount) + sizeof(colCount) + sizeof(bitmapSize))
    return 0;

  // Helper variables for deserialization
  size_t index = 0;
  size_t totalWrite = 0;
  size_t objSize = 0;

  // Deserialize rowCount
  objSize = sizeof(rowCount);
  memcpy(&rowCount, input, objSize);
  index += objSize;
  totalWrite += objSize;

  // Deserialize colCount
  objSize = sizeof(colCount);
  memcpy(&colCount, input + index, objSize);
  index += objSize;
  totalWrite += objSize;

  // Deserialize bitmapSize
  objSize = sizeof(bitmapSize);
  memcpy(&bitmapSize, input + index, objSize);
  index += objSize;
  totalWrite += objSize;

  // Now validate the full size including pin data
  size_t expectedSize = sizeof(rowCount) + sizeof(colCount) + sizeof(bitmapSize) +
                        rowCount + colCount + sizeof(refreshRate) +
                        sizeof(bitMapSendRate);
  if (size < expectedSize)
    return 0;

  // Resize vectors BEFORE copying data into them
  rowPins.resize(rowCount);
  objSize = rowCount;
  memcpy(rowPins.data(), input + index, objSize);
  index += rowCount;
  totalWrite += objSize;

  // Resize vectors BEFORE copying data into them
  colPins.resize(colCount);
  objSize = colCount;
  memcpy(colPins.data(), input + index, objSize);
  index += colCount;
  totalWrite += objSize;

  // Deserialize refreshRate
  objSize = sizeof(refreshRate);
  memcpy(&refreshRate, input + index, objSize);
  index += objSize;
  totalWrite += objSize;

  // Deserialize bitMapSendFrequency
  objSize = sizeof(bitMapSendRate);
  memcpy(&bitMapSendRate, input + index, objSize);
  totalWrite += objSize;

  return totalWrite;
}

size_t KeyScannerConfig::getSerializedSize() const
{
  // Return the total size needed for serialization
  return sizeof(rowCount) + sizeof(colCount) + sizeof(bitmapSize) + rowCount + colCount +
         sizeof(refreshRate) + sizeof(bitMapSendRate);
}

uint8_t KeyScannerConfig::getHIDCodeForIndex(uint8_t localKeyIndex) const
{
  if (localKeyIndex >= localToHidMap.size()) {
    log.warn("Requested HID code for out-of-bounds index %d", localKeyIndex);
    return 0;
  }
  return localToHidMap[localKeyIndex];
}