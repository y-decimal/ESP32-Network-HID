#ifndef CONFIGTYPES_H
#define CONFIGTYPES_H

#include <cstring>
#include <interfaces/ISerializableStructs.h>
#include <shared/CommTypes.h>
#include <stdint.h>
#include <vector>

using pinType = std::vector<uint8_t>;
using countType = uint8_t;

static constexpr uint8_t MAX_PIN_COUNT = 20;

struct GlobalConfig {
  DeviceRole roles[(size_t)DeviceRole::Count]{};
  MacAddress deviceMac{};

  void setRoles(DeviceRole *roleArray, size_t arrSize) {
    if (arrSize > (size_t)DeviceRole::Count)
      return;
    memcpy(roles, roleArray, arrSize);
  }

  void setMac(MacAddress mac) { memcpy(deviceMac, mac, 6); }

  void getRoles(DeviceRole *out, size_t size) {
    if (size < sizeof(roles))
      return;
    memcpy(out, roles, sizeof(roles));
  }

  void getMac(uint8_t *out, size_t size) {
    if (size < sizeof(deviceMac))
      return;
    memcpy(out, deviceMac, sizeof(deviceMac));
  }
};

struct KeyScannerConfig {
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
               uint8_t colSize) {
    rowPins.assign(rowPinData, rowPinData + rowSize);
    rows = rowSize;
    colPins.assign(colPinData, colPinData + colSize);
    cols = colSize;
    bitMapSize = (rows * cols + 7) / 8;
  }

  void setRefreshRate(uint16_t rate) {
    if (rate < MIN_REFRESH_RATE || rate > MAX_REFRESH_RATE)
      return;
    refreshRate = rate;
  }

  void setBitMapSendInterval(uint16_t rateDivisor) {
    if (MIN_BITMAP_INTERVAL < 2 || rateDivisor > MAX_BITMAP_INTERVAL)
      // Limited to a range of 2-5000, to ensure bitmaps aren't sent every
      // single loop (as this would most likely block the ESP communication),
      // and 5000 to ensure we don't exceed the uint16_t limit and also stay
      // within reasonable bounds for regular bitMap refreshes
      return;
    bitMapSendInterval = rateDivisor;
  }

  void setConfig(KeyCfgParams config) {
    setPins(config.rowPins, config.rows, config.colPins, config.cols);
    setRefreshRate(config.refreshRate);
    setBitMapSendInterval(config.bitMapSendInterval);
  }

  pinType getRowPins() const { return rowPins; }
  pinType getColPins() const { return colPins; }
  countType getRowsCount() const { return rows; }
  countType getColCount() const { return cols; }
  uint8_t getBitmapSize() const { return bitMapSize; }
  uint16_t getRefreshRate() const { return refreshRate; }
  uint16_t getBitMapSendInterval() const { return bitMapSendInterval; }
};

#endif