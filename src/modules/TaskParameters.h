#ifndef TASKPARAMETERS_H
#define TASKPARAMETERS_H

#include <shared/DataTypes.h>
#include <submodules/ConfigManager.h>

struct KeyScannerParameters {
  KeyScannerConfig *config = nullptr;
  KeyScannerState *state = nullptr;
};

struct BitMapSenderParameters {
  using BitmapSendCallback = void (*)(const uint8_t *data, uint8_t size);

  BitMapSenderConfig *config = nullptr;
  KeyScannerState *state = nullptr;
  BitmapSendCallback callback = nullptr;
};

#endif