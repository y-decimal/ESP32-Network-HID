#ifndef CONFIGTYPES_H
#define CONFIGTYPES_H

#include <cstring>
#include <shared/CommTypes.h>
#include <stdint.h>

using pinType = uint8_t;
using countType = uint8_t;

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

  void setBitMapSendInterval(uint16_t rateDivisor) {
    if (rateDivisor < 2 || rateDivisor > 5000)
      // Limited to a range of 2-5000, to ensure bitmaps aren't sent every
      // single loop (as this would most likely block the ESP communication),
      // and 5000 to ensure we don't exceed the uint16_t limit and also stay
      // within reasonable bounds for regular bitMap refreshes
      return;
    bitMapSendInterval = rateDivisor;
  }

  uint16_t getRefreshRate() const { return refreshRate; }
  uint16_t getBitMapSendInterval() const { return bitMapSendInterval; }

private:
  uint16_t refreshRate = 1;
  uint16_t bitMapSendInterval = 5;
};

#endif