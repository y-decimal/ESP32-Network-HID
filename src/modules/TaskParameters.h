#ifndef TASKPARAMETERS_H
#define TASKPARAMETERS_H

#include <shared/ConfigTypes.h>
#include <shared/DataTypes.h>

struct KeyScannerParameters {
  KeyScannerConfig config{};
  KeyScannerState *state = nullptr;
};

struct BitMapSenderParameters {
  using BitmapSendCallback = void (*)(const uint8_t *data, uint8_t size);

  BitMapSenderConfig config{};
  KeyScannerState *state = nullptr;
  BitmapSendCallback callback = nullptr;
};

#endif