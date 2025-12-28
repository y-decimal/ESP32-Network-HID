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
  countType rows = 0;
  countType cols = 0;
  pinType rowPins[MAX_PIN_COUNT]{};
  pinType colPins[MAX_PIN_COUNT]{};
  uint16_t refreshRate = 1;

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
};

struct BitMapSenderConfig {
  uint16_t refreshRate = 1;
};

#endif