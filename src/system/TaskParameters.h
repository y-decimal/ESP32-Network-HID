#ifndef TASKPARAMETERS_H
#define TASKPARAMETERS_H

#include <queue.h>
#include <submodules/ConfigManager/ConfigManager.h>

class ConfigManager; // Forward declaration

struct KeyScannerParameters {
  ConfigManager *configManager = nullptr;
  QueueHandle_t eventQueueHandle = nullptr;
};

#endif