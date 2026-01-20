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

static Logger taskLog("TaskManager");

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

  enum TaskId : uint32_t
  {
    EVENT_BUS_TASK = 1 << 0,
    LOGGER_TASK = 1 << 1,
    KEYSCANNER_TASK = 1 << 2,
    MASTER_TASK = 1 << 3,
    SLAVE_TASK = 1 << 4
  };

  TaskManager(Platform &platform)
      : platform(platform),
        configManager(&platform.storage),
        eventBusTask(),
        loggerTask(),
        keyScannerTask(configManager, platform.gpio),
        masterTask(platform.transport),
        slaveTask(platform.transport)
  {
  }

  void start();

private:
  Platform &platform;
  ConfigManager configManager;

  // Task classes are stored here
  EventBusTask eventBusTask;
  LoggerTask loggerTask;
  KeyScannerTask keyScannerTask;
  MasterTask masterTask;
  SlaveTask slaveTask;

  // Bitmap of currently active tasks
  uint32_t currentTasks = 0;

  void startModules(uint32_t moduleBitmap);
  void stopTaskByBit(uint32_t bit);
  void restartTaskByBit(uint32_t bit);
  void startTaskByBit(uint32_t bit);
  uint32_t getAllRequiredTasks();
};

#endif