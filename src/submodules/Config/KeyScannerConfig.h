#ifndef KEYSCANNERCONFIG_H
#define KEYSCANNERCONFIG_H

#include <interfaces/IConfig.h>
#include <stdint.h>
#include <vector>

using pinType = std::vector<uint8_t>;
using countType = uint8_t;

/**
 * @brief Key Scanner configuration class managing key matrix settings.
 *
 * The KeyScannerConfig class provides methods to set and get key matrix
 * configurations such as row/column pins, refresh rate, and bitmap send
 * frequency. It also implements serialization and deserialization methods
 * for storing and retrieving configuration data.
 */
class KeyScannerConfig : public IConfig
{
private:
  // Key matrix configuration parameters
  countType rowCount = 0;
  countType colCount = 0;
  pinType rowPins{};
  pinType colPins{};

  // Key scanning parameters
  uint8_t bitmapSize = 0;
  uint16_t refreshRate = 100;
  uint16_t bitMapSendRate = 5;

  // Local index to HID code mapping
  std::vector<uint8_t> localToHidMap{};

  // Configuration constraints
  static constexpr const uint16_t MIN_REFRESH_RATE = 1;
  static constexpr const uint16_t MAX_REFRESH_RATE = 1000;
  static constexpr const uint16_t MIN_BITMAP_REFRESH_RATE = 1;
  static constexpr const uint16_t MAX_BITMAP_REFRESH_RATE = 500;
  static constexpr const size_t MAX_PIN_COUNT = 20;
  static constexpr const size_t MAX_KEY_COUNT = 128;

public:
  // Definition of the configuration structure
  struct KeyCfgParams
  {
    countType rowCount;
    countType colCount;
    uint8_t *rowPins;
    uint8_t *colPins;
    uint16_t refreshRate;
    uint16_t bitmapSendRate;
    uint8_t *localToHidMap; // Size should be rowCount * colCount
  };

  /**
   * @brief Set the row and column pins.
   * @param rowPinData Array of row pin numbers.
   * @param rowSize Number of row pins.
   * @param colPinData Array of column pin numbers.
   * @param colSize Number of column pins.
   */
  void setPins(uint8_t *rowPinData, uint8_t rowSize, uint8_t *colPinData,
               uint8_t colSize);

  /**
   * @brief Set the refresh rate in Hz.
   * @param rate Refresh rate in Hz (1-1000)
   */
  void setRefreshRate(uint16_t rate);

  /**
   * Set bitmap send frequency in Hz.
   * The actual loop interval is calculated as: loopsPerBitmap = refreshRate /
   * bitmapSendRate
   * @param frequency Bitmap frequency in Hz (1-500)
   */
  void setBitmapSendFrequency(uint16_t frequency);

  /**
   * @brief Set the local to HID mapping.
   * @param mapData Array of local to HID mapping data.
   * @param mapSize Size of the mapping data array.
   */
  void setLocalToHidMap(uint8_t *mapData, size_t mapSize);

  /**
   * @brief Update the HID code for a specific local key index.
   * @param localKeyIndex Local key index to update.
   * @param hidCode New HID code to set.
   */
  void updateHIDCodeForIndex(uint8_t localKeyIndex, uint8_t hidCode);

  /**
   * @brief Set the entire key scanner configuration.
   * @param config KeyCfgParams structure containing configuration data.
   */
  void setConfig(KeyCfgParams config);

  /**
   * @brief Get the row pins.
   * @return Vector of row pin numbers.
   */
  pinType getRowPins() const { return rowPins; }

  /**
   * @brief Get the column pins.
   * @return Vector of column pin numbers.
   */
  pinType getColPins() const { return colPins; }

  /**
   * @brief Get the number of rowCount.
   * @return Number of rowCount.
   */
  countType getRowsCount() const { return rowCount; }

  /**
   * @brief Get the number of columns.
   * @return Number of columns.
   */
  countType getColCount() const { return colCount; }

  /**
   * @brief Get the bitmap size.
   * @return Bitmap size.
   */
  uint8_t getBitmapSize() const { return bitmapSize; }

  /**
   * @brief Get the refresh rate.
   * @return Refresh rate in Hz.
   */
  uint16_t getRefreshRate() const { return refreshRate; }

  /**
   * @brief Get the bitmap send rate.
   * @return Bitmap send rate in Hz.
   */
  uint16_t getBitmapSendRate() const { return bitMapSendRate; }

  /**
   * @brief Get the local to HID mapping.
   * @return Vector of local to HID mapping data.
   */
  std::vector<uint8_t> getLocalToHidMap() const { return localToHidMap; }

  /**
   * @brief Get the HID code for a local key index from the map
   * @return HID code
   */
  uint8_t getHIDCodeForIndex(uint8_t localKeyIndex) const;

  // Implementation of IConfig interface methods
  static constexpr const char *NAMESPACE = "KeyCfg";
  const char *getNamespace() { return NAMESPACE; }
  bool save() override;
  bool load() override;
  bool erase() override;

  // Implementation of ISerializable interface methods
  size_t packSerialized(uint8_t *output, size_t size) const override;
  size_t unpackSerialized(const uint8_t *input, size_t size) override;
  size_t getSerializedSize() const override;
};

#endif