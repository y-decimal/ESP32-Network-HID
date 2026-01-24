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
      frequency > MAX_BITMAP_REFRESH_RATE)
  {
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
  if (mapSize > MAX_KEY_COUNT)
  {
    log.warn("Local to HID map size %zu exceeds maximum of %d", mapSize, MAX_KEY_COUNT);
    return;
  }
  localToHidMap.assign(mapData, mapData + mapSize);
}

void KeyScannerConfig::updateHIDCodeForIndex(uint8_t localKeyIndex, uint8_t hidCode)
{
  if (localKeyIndex >= localToHidMap.size())
  {
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

bool KeyScannerConfig::save()
{
  if (storage == nullptr)
  {
    log.error("No storage backend set, cannot save config");
    return false;
  }

  size_t ownSize = getSerializedSize();
  uint8_t *buffer = (uint8_t *)malloc(ownSize);
  size_t packedSize = packSerialized(buffer, ownSize);
  if (packedSize != ownSize)
    log.warn("Packed size %zu and serialized size size %zu don't match!", packedSize, ownSize);

  bool success = storage->save(NAMESPACE, buffer, ownSize);
  free(buffer);

  success ? log.info("Configuration saved") : log.error("Saving configuration failed");

  return success;
}

bool KeyScannerConfig::load()
{
  if (storage == nullptr)
  {
    log.error("No storage backend set, cannot load config");
    return false;
  }

  size_t ownSize = storage->getSize(NAMESPACE);
  if (ownSize == 0)
  {
    log.error("No config data stored");
    return false;
  }

  uint8_t *buffer = (uint8_t *)malloc(ownSize);
  bool success = storage->load(NAMESPACE, buffer, ownSize);

  if (!success)
  {
    log.error("Loading config data failed");
    free(buffer);
    return false;
  }

  size_t unpackedSize = unpackSerialized(buffer, ownSize);

  if (unpackedSize != ownSize)
  {
    log.warn("Unpacked size %zu and loaded size %zu don't match!", unpackedSize, ownSize);
  }

  free(buffer);

  return success;
}

bool KeyScannerConfig::erase()
{
  if (storage == nullptr)
  {
    log.error("No storage backend set, cannot erase config");
    return false;
  }

  bool success = storage->remove(NAMESPACE);

  success ? log.info("Configuration erased") : log.error("Erasing configuration failed");

  return success;
}

size_t KeyScannerConfig::packSerialized(uint8_t *output, size_t size) const
{

  // Check if provided buffer is large enough
  size_t ownSize = getSerializedSize();
  if (size < ownSize)
    return 0;

  // Helper variables for serialization
  size_t totalWrite = 0;
  size_t objSize = 0;

  objSize = sizeof(size_t);
  memcpy(output + totalWrite, &ownSize, objSize);
  totalWrite += objSize;

  // Serialize rowCount
  objSize = sizeof(rowCount);
  memcpy(output + totalWrite, &rowCount, objSize);
  totalWrite += objSize;

  // Serialize colCount
  objSize = sizeof(colCount);
  memcpy(output + totalWrite, &colCount, objSize);
  totalWrite += objSize;

  // Serialize bitmapSize
  objSize = sizeof(bitmapSize);
  memcpy(output + totalWrite, &bitmapSize, objSize);
  totalWrite += objSize;

  // Serialize rowPins
  objSize = rowCount;
  memcpy(output + totalWrite, rowPins.data(), objSize);
  totalWrite += objSize;

  // Serialize colPins
  objSize = colCount;
  memcpy(output + totalWrite, colPins.data(), objSize);
  totalWrite += objSize;

  // Serialize refreshRate
  objSize = sizeof(refreshRate);
  memcpy(output + totalWrite, &refreshRate, objSize);
  totalWrite += objSize;

  // Serialize bitMapSendFrequency
  objSize = sizeof(bitMapSendRate);
  memcpy(output + totalWrite, &bitMapSendRate, objSize);
  totalWrite += objSize;

  // Serialize localToHidMap
  objSize = rowCount * colCount;
  memcpy(output + totalWrite, localToHidMap.data(), objSize);
  totalWrite += objSize;

  return totalWrite;
}

size_t KeyScannerConfig::unpackSerialized(const uint8_t *input, size_t size)
{
  // Helper variables for deserialization
  size_t totalRead = 0;
  size_t objSize = 0;

  size_t ownSize = 0;
  objSize = sizeof(size_t);
  memcpy(&ownSize, input + totalRead, objSize);
  totalRead += objSize;

  if (size < ownSize)
  {
    log.error("Could not unpack config into buffer. Required size: %zu, actual size: %zu", ownSize, size);
    return 0;
  }

  // Deserialize rowCount
  objSize = sizeof(rowCount);
  memcpy(&rowCount, input + totalRead, objSize);
  totalRead += objSize;

  // Deserialize colCount
  objSize = sizeof(colCount);
  memcpy(&colCount, input + totalRead, objSize);
  totalRead += objSize;

  // Deserialize bitmapSize
  objSize = sizeof(bitmapSize);
  memcpy(&bitmapSize, input + totalRead, objSize);
  totalRead += objSize;

  // Resize vectors BEFORE copying data into them
  rowPins.resize(rowCount);
  objSize = rowCount;
  memcpy(rowPins.data(), input + totalRead, objSize);
  totalRead += objSize;

  // Resize vectors BEFORE copying data into them
  colPins.resize(colCount);
  objSize = colCount;
  memcpy(colPins.data(), input + totalRead, objSize);
  totalRead += objSize;

  // Deserialize refreshRate
  objSize = sizeof(refreshRate);
  memcpy(&refreshRate, input + totalRead, objSize);
  totalRead += objSize;

  // Deserialize bitMapSendFrequency
  objSize = sizeof(bitMapSendRate);
  memcpy(&bitMapSendRate, input + totalRead, objSize);
  totalRead += objSize;

  // Deserialize localToHidMap
  objSize = rowCount * colCount;
  localToHidMap.resize(objSize);
  memcpy(localToHidMap.data(), input + totalRead, objSize);
  totalRead += objSize;

  return totalRead;
}

size_t KeyScannerConfig::getSerializedSize() const
{
  // Return the total size needed for serialization
  // rowCount and colCount act as rowPins and colPins size metadata
  // rowCount * colCount acts as localToHidMap size metadata
  // First field for total config size information
  return sizeof(size_t) + sizeof(rowCount) + sizeof(colCount) +
         sizeof(bitmapSize) + rowCount + colCount +
         sizeof(refreshRate) + sizeof(bitMapSendRate) + localToHidMap.size();
}

uint8_t KeyScannerConfig::getHIDCodeForIndex(uint8_t localKeyIndex) const
{
  if (localKeyIndex >= localToHidMap.size())
  {
    log.warn("Requested HID code for out-of-bounds index %d", localKeyIndex);
    return 0;
  }
  return localToHidMap[localKeyIndex];
}