#ifndef KEYSCANNERCONFIG_H
#define KEYSCANNERCONFIG_H

#include <interfaces/ISerializableStructs.h>
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
class KeyScannerConfig : public Serializable {
private:
  // Key matrix configuration parameters
  countType rows = 0;
  countType cols = 0;
  pinType rowPins;
  pinType colPins;

  // Key scanning parameters
  uint8_t bitMapSize = 0;
  uint16_t refreshRate = 1;
  uint16_t bitMapSendFrequency = 5;

  // Configuration constraints
  static constexpr uint16_t MIN_REFRESH_RATE = 1;
  static constexpr uint16_t MAX_REFRESH_RATE = 1000;
  static constexpr uint16_t MIN_BITMAP_REFRESH_RATE = 1;
  static constexpr uint16_t MAX_BITMAP_REFRESH_RATE = 500;
  static constexpr size_t MAX_PIN_COUNT = 20;

  // Maximum size for serialized configuration
  static constexpr size_t MAX_KEYSCANNER_CONFIG_SIZE =
      sizeof(rows) + sizeof(cols) + sizeof(bitMapSize) + MAX_PIN_COUNT * 2 +
      sizeof(refreshRate) + sizeof(bitMapSendFrequency);

public:
  // Definition of the configuration structure
  struct KeyCfgParams {
    countType rows;
    countType cols;
    uint8_t *rowPins;
    uint8_t *colPins;
    uint16_t refreshRate;
    uint16_t bitMapSendInterval;
  };

  // Definition of the serialized configuration structure
  struct SerializedConfig {
    uint8_t data[MAX_KEYSCANNER_CONFIG_SIZE];
    size_t size = MAX_KEYSCANNER_CONFIG_SIZE;
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
   * bitMapSendInterval
   * @param frequency Bitmap frequency in Hz (1-500)
   */
  void setBitmapSendFrequency(uint16_t frequency);

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
   * @brief Get the number of rows.
   * @return Number of rows.
   */
  countType getRowsCount() const { return rows; }

  /**
   * @brief Get the number of columns.
   * @return Number of columns.
   */
  countType getColCount() const { return cols; }

  /**
   * @brief Get the bitmap size.
   * @return Bitmap size.
   */
  uint8_t getBitmapSize() const { return bitMapSize; }

  /**
   * @brief Get the refresh rate.
   * @return Refresh rate in Hz.
   */
  uint16_t getRefreshRate() const { return refreshRate; }

  /**
   * @brief Get the bitmap send interval.
   * @return Bitmap send interval in Hz.
   */
  uint16_t getBitMapSendInterval() const { return bitMapSendFrequency; }

  // Implementation of Serializable interface methods
  size_t packSerialized(uint8_t *output, size_t size) const override;
  size_t unpackSerialized(const uint8_t *input, size_t size) override;
  size_t getSerializedSize() const override;
};

#endif