#ifndef CONFIGTYPES_G
#define CONFIGTYPES_H

#include <shared/CommTypes.h>
#include <stdint.h>

using pinType = uint8_t;
using countType = uint8_t;

static constexpr uint8_t MAX_PIN_COUNT = UINT8_MAX;

struct HardwareConfig {

  struct KeyMatrixConfig {
    countType rows;
    countType cols;
    pinType rowPins[MAX_PIN_COUNT];
    pinType colPins[MAX_PIN_COUNT];
  };

  struct InternalPinConfig {
    pinType internalLedPin;
    pinType internalRgbPin;
  };

  //

  KeyMatrixConfig keyMatrix;
  InternalPinConfig internalPin;
};

struct UserConfig {
  DeviceRole role;
};

#endif