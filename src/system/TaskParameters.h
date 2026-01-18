#ifndef TASKPARAMETERS_H
#define TASKPARAMETERS_H

#include <interfaces/IEspNow.h>
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

struct SlaveEspParameters
{
  QueueHandle_t keyEventQueue = nullptr;
  IEspNow *espNow = nullptr;
};

struct MasterEspParameters
{
  QueueHandle_t eventBusQueue = nullptr;
  IEspNow *espNow = nullptr;
};

#endif