#ifndef CONFIGTYPES_H
#define CONFIGTYPES_H

#include <cstring>
#include <shared/CommTypes.h>
#include <stdint.h>

using pinType = uint8_t;
using countType = uint8_t;

static constexpr uint8_t MAX_PIN_COUNT = 50;

struct GlobalConfig {
  DeviceRole roles[(size_t)DeviceRole::Count]{};
  MacAddress deviceMac{};

  void setRoles(DeviceRole *roleArray, size_t arrSize) {
    if (arrSize > (size_t)DeviceRole::Count)
      return;
    memcpy(roles, roleArray, arrSize);
  }
  void setMac(MacAddress mac) { memcpy(deviceMac, mac, 6); }
};

struct KeyScannerConfig {
  static constexpr uint16_t MIN_REFRESH_RATE = 1;
  static constexpr uint16_t MAX_REFRESH_RATE = 4000;

  countType rows = 0;
  countType cols = 0;
  pinType rowPins[MAX_PIN_COUNT]{};
  pinType colPins[MAX_PIN_COUNT]{};

  void setRowPins(pinType *rowPinArray, size_t arrSize) {
    if (arrSize > MAX_PIN_COUNT)
      return;
    memcpy(rowPins, rowPinArray, arrSize);
  }

  void setColPins(pinType *colPinArray, size_t arrSize) {
    if (arrSize > MAX_PIN_COUNT)
      return;
    memcpy(colPins, colPinArray, arrSize);
  }

  void setRefreshRate(uint16_t rate) {
    if (rate < MIN_REFRESH_RATE || rate > MAX_REFRESH_RATE)
      return;
    refreshRate = rate;
  }

  void setBitMapSendRatio(uint16_t rateDivisor) {
    if (rateDivisor < 2 || rateDivisor > 5000)
      return;
    bitMapSendRatio = rateDivisor;
  }

  uint16_t getRefreshRate() const { return refreshRate; }
  uint16_t getBitMapSendRatio() const { return bitMapSendRatio; }

private:
  uint16_t refreshRate = 1;
  uint16_t bitMapSendRatio = 5;
};

#endif