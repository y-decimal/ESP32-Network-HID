#ifndef KEYSCANNERCONFIG_H
#define KEYSCANNERCONFIG_H

#include <interfaces/ISerializableStructs.h>
#include <stdint.h>
#include <vector>

using pinType = std::vector<uint8_t>;
using countType = uint8_t;

class KeyScannerConfig {
private:
  countType rows = 0;
  countType cols = 0;
  uint8_t bitMapSize = 0;
  pinType rowPins;
  pinType colPins;
  uint16_t refreshRate = 1;
  uint16_t bitMapSendInterval = 5;

  static constexpr uint16_t MIN_REFRESH_RATE = 1;
  static constexpr uint16_t MAX_REFRESH_RATE = 4000;
  static constexpr uint16_t MIN_BITMAP_INTERVAL = 2;
  static constexpr uint16_t MAX_BITMAP_INTERVAL = 5000;

public:
  struct KeyCfgParams {
    countType rows;
    countType cols;
    uint8_t *rowPins;
    uint8_t *colPins;
    uint16_t refreshRate;
    uint16_t bitMapSendInterval;
  };

  void setPins(uint8_t *rowPinData, uint8_t rowSize, uint8_t *colPinData,
               uint8_t colSize);

  void setRefreshRate(uint16_t rate);

  void setBitMapSendInterval(uint16_t rateDivisor);

  void setConfig(KeyCfgParams config);

  pinType KeyScannerConfig::getRowPins() const { return rowPins; }
  pinType KeyScannerConfig::getColPins() const { return colPins; }
  countType KeyScannerConfig::getRowsCount() const { return rows; }
  countType KeyScannerConfig::getColCount() const { return cols; }
  uint8_t KeyScannerConfig::getBitmapSize() const { return bitMapSize; }
  uint16_t KeyScannerConfig::getRefreshRate() const { return refreshRate; }
  uint16_t KeyScannerConfig::getBitMapSendInterval() const {
    return bitMapSendInterval;
  }
};

#endif