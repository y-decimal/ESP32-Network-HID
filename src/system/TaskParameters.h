#ifndef TASKPARAMETERS_H
#define TASKPARAMETERS_H

#include <interfaces/IEspNow.h>
#include <interfaces/IGpio.h>
#include <queue.h>
#include <submodules/ConfigManager/ConfigManager.h>

class ConfigManager; // Forward declaration

struct KeyScannerParameters {
  ConfigManager *configManager = nullptr;
  QueueHandle_t eventBusHandle = nullptr;
  IGpio *gpio = nullptr;
};

struct slaveEspParameters {
  QueueHandle_t keyEventHandle = nullptr;
  IEspNow *espNow = nullptr;
};

#endif