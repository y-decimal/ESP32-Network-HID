#ifndef CONFIGTYPES_H
#define CONFIGTYPES_H

#include <cstring>
#include <interfaces/ISerializableStructs.h>
#include <shared/CommTypes.h>
#include <stdint.h>

using pinType = uint8_t;
using countType = uint8_t;

static constexpr uint8_t MAX_PIN_COUNT = 50;

struct GlobalConfig : Serializable {
  DeviceRole roles[(size_t)DeviceRole::Count]{};
  MacAddress deviceMac{};

  void setRoles(DeviceRole *roleArray, size_t arrSize) {
    if (arrSize > (size_t)DeviceRole::Count)
      return;
    memcpy(roles, roleArray, arrSize);
  }
  void setMac(MacAddress mac) { memcpy(deviceMac, mac, 6); }
};

struct KeyScannerConfig : Serializable {
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

  size_t pack(uint8_t *output) {

    size_t actualRowPinBytes = 0;
    for (size_t i; i < MAX_PIN_COUNT; i++) {
      if (rowPins[i] != 0)
        actualRowPinBytes++;
    }

    size_t actualColPinBytes = 0;
    for (size_t i; i < MAX_PIN_COUNT; i++) {
      if (colPins[i] != 0)
        actualColPinBytes++;
    }

    constexpr uint8_t bitSpacePinArray = MAX_PIN_COUNT / 8;

    size_t byteSize = 2 * bitSpacePinArray + actualRowPinBytes +
                      actualColPinBytes + sizeof(rows) + sizeof(cols) +
                      sizeof(refreshRate);

    uint8_t buffer[byteSize];

    // Finish implementation later
  }
};

#endif