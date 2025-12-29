#ifndef TASKPARAMETERS_H
#define TASKPARAMETERS_H

#include <shared/ConfigTypes.h>
#include <shared/DataTypes.h>

class ConfigManager; // Forward declaration

struct KeyScannerParameters {
  ConfigManager *configManager = nullptr;
  KeyScannerState *state = nullptr;
};

struct BitMapSenderParameters {
  using BitmapSendCallback = void (*)(const uint8_t *data, uint8_t size);

  ConfigManager *configManager = nullptr;
  KeyScannerState *state = nullptr;
  BitmapSendCallback callback = nullptr;
};

#endif