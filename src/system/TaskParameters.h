#ifndef TASKPARAMETERS_H
#define TASKPARAMETERS_H

#include <interfaces/ITransport.h>
#include <interfaces/IGpio.h>
#include <queue.h>
#include <submodules/ConfigManager/ConfigManager.h>

class ConfigManager; // Forward declaration

struct KeyScannerParameters
{
  ConfigManager *configManager = nullptr;
  QueueHandle_t eventBusQueue = nullptr;
  IGpio *gpio = nullptr;
};

struct MasterSlaveParameters
{
  QueueHandle_t eventBusQueue = nullptr;
  ITransport *espNow = nullptr;
};

#endif