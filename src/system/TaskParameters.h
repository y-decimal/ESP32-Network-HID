#ifndef TASKPARAMETERS_H
#define TASKPARAMETERS_H

#include <queue.h>
#include <shared/ConfigTypes.h>
#include <shared/DataTypes.h>

class ConfigManager; // Forward declaration

struct KeyScannerParameters {
  ConfigManager *configManager = nullptr;
  KeyScannerState *state = nullptr;
  QueueHandle_t eventQueueHandle = nullptr;
};

struct BitMapSenderParameters {
  ConfigManager *configManager = nullptr;
  KeyScannerState *state = nullptr;
  IBitMapRoutingStrategy *routing = nullptr;
};

#endif