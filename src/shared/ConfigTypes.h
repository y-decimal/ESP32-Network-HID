#ifndef CONFIGTYPES_H
#define CONFIGTYPES_H

#include <cstring>
#include <shared/CommTypes.h>
#include <stdint.h>

using pinType = uint8_t;
using countType = uint8_t;

static constexpr uint8_t MAX_PIN_COUNT = 50;

struct GlobalConfig {
  DeviceRole roles[(size_t)DeviceRole::Count];
  MacAddress deviceMac;
};

struct KeyScannerConfig {
  countType rows;
  countType cols;
  pinType rowPins[MAX_PIN_COUNT];
  pinType colPins[MAX_PIN_COUNT];
  uint16_t refreshRate;
};

#endif