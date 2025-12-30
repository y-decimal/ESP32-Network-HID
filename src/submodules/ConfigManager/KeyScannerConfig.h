#ifndef KEYSCANNERCONFIG_H
#define KEYSCANNERCONFIG_H

#include <interfaces/ISerializableStructs.h>
#include <stdint.h>
#include <vector>

using pinType = std::vector<uint8_t>;
using countType = uint8_t;

class KeyScannerConfig : public Serializable {
private:
  countType rows = 0;
  countType cols = 0;
  uint8_t bitMapSize = 0;
  pinType rowPins;
  pinType colPins;
  uint16_t refreshRate = 1;
  uint16_t bitMapSendFrequency = 5;

  static constexpr uint16_t MIN_REFRESH_RATE = 1;
  static constexpr uint16_t MAX_REFRESH_RATE = 4000;
  static constexpr uint16_t MIN_BITMAP_REFRESH_RATE = 1;
  static constexpr uint16_t MAX_BITMAP_REFRESH_RATE = 500;
  static constexpr size_t MAX_PIN_COUNT = 20;
  static constexpr size_t MAX_KEYSCANNER_CONFIG_SIZE =
      sizeof(rows) + sizeof(cols) + sizeof(bitMapSize) + MAX_PIN_COUNT * 2 +
      sizeof(refreshRate) + sizeof(bitMapSendFrequency);

public:
  struct KeyCfgParams {
    countType rows;
    countType cols;
    uint8_t *rowPins;
    uint8_t *colPins;
    uint16_t refreshRate;
    uint16_t bitMapSendInterval;
  };

  struct SerializedConfig {
    uint8_t data[MAX_KEYSCANNER_CONFIG_SIZE];
    size_t size = MAX_KEYSCANNER_CONFIG_SIZE;
  };

  void setPins(uint8_t *rowPinData, uint8_t rowSize, uint8_t *colPinData,
               uint8_t colSize);

  void setRefreshRate(uint16_t rate);

  /**
   * Set bitmap send frequency in Hz.
   * The actual loop interval is calculated as: loopsPerBitmap = refreshRate /
   * bitMapSendInterval
   * @param frequency Bitmap frequency in Hz (1-500)
   */
  void setBitmapSendFrequency(uint16_t frequency);

  void setConfig(KeyCfgParams config);

  pinType getRowPins() const { return rowPins; }
  pinType getColPins() const { return colPins; }
  countType getRowsCount() const { return rows; }
  countType getColCount() const { return cols; }
  uint8_t getBitmapSize() const { return bitMapSize; }
  uint16_t getRefreshRate() const { return refreshRate; }
  uint16_t getBitMapSendInterval() const { return bitMapSendFrequency; }

  size_t packSerialized(uint8_t *output, size_t size) const override;
  size_t unpackSerialized(const uint8_t *input, size_t size) override;
  size_t getSerializedSize() const override;
};

#endif