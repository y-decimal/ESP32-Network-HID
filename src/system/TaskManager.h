#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <FreeRTOS.h>

#include <modules/EventBusTask.h>
#include <modules/LoggerTask.h>
#include <modules/KeyScannerTask.h>
#include <modules/MasterTask.h>
#include <modules/SlaveTask.h>

#include <submodules/Config/ConfigManager.h>
#include <submodules/Logger.h>
#include <system/SystemConfig.h>

#include <interfaces/IGpio.h>
#include <interfaces/ITransport.h>
#include <interfaces/ILogSink.h>
#include <interfaces/IStorage.h>

using DeviceModule = GlobalConfig::DeviceModule;
using DeviceMode = GlobalConfig::DeviceMode;

class TaskManager
{
public:
  struct Platform
  {
    IGpio &gpio;
    ITransport &transport;
    IStorage &storage;
  };

  enum TaskId : uint32_t // Define task IDs as bit flags, determines starting/stopping order
  {
    LOGGER_TASK = 1 << 0,
    EVENT_BUS_TASK = 1 << 1,
    MASTER_TASK = 1 << 2,
    SLAVE_TASK = 1 << 3,
    KEYSCANNER_TASK = 1 << 4
  };

  TaskManager(Platform &platform)
      : platform(platform),
        configManager(&platform.storage),
        loggerTask(),
        eventBusTask(),
        masterTask(platform.transport),
        slaveTask(platform.transport),
        keyScannerTask(configManager, platform.gpio)
  {
  }

  void start();
  ConfigManager &getConfigManagerCopy();

private:
  Platform &platform;
  ConfigManager configManager;

  // Task classes are stored here, this is also the order they will be started/stopped
  LoggerTask loggerTask;
  EventBusTask eventBusTask;
  MasterTask masterTask;
  SlaveTask slaveTask;
  KeyScannerTask keyScannerTask;

  // Bitmap of currently active tasks
  uint32_t currentTasks = 0;
  uint32_t coreModules = (TaskId::EVENT_BUS_TASK | TaskId::LOGGER_TASK);

  void startModules(uint32_t moduleBitmap);
  void stopTaskByBit(uint32_t bit);
  void restartTaskByBit(uint32_t bit);
  void startTaskByBit(uint32_t bit);
  uint32_t getAllRequiredTasks();
};

#endif