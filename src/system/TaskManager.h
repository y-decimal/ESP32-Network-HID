#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <FreeRTOS.h>

#include <modules/EventBusTask.h>
#include <modules/LoggerTask.h>
#include <modules/KeyScannerTask.h>
#include <modules/MasterTask.h>
#include <modules/SlaveTask.h>

#include <submodules/ConfigManager/ConfigManager.h>
#include <submodules/Logger.h>
#include <system/SystemConfig.h>

#include <interfaces/IGpio.h>
#include <interfaces/ITransport.h>
#include <interfaces/ILogSink.h>
#include <interfaces/IStorage.h>

static Logger taskLog("TaskManager");

class TaskManager
{
public:
  struct Platform
  {
    IGpio &gpio;
    ITransport &transport;
    IStorage &storage;
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

  void start()
  {
    loggerTask.start({STACK_LOGGER, PRIORITY_LOGGER, CORE_LOGGER});
    eventBusTask.start({STACK_EVENTBUS, PRIORITY_EVENTBUS, CORE_EVENTBUS});

    bool configLoaded = configManager.loadConfig();
    if (!configLoaded)
      taskLog.warn("Failed to load configuration, using defaults");
    DeviceRole roles[(size_t)DeviceRole::Count];
    configManager.getConfig<GlobalConfig>().getRoles(roles, (size_t)DeviceRole::Count);
    for (size_t i = 0; i < (size_t)DeviceRole::Count; i++)
    {
      taskLog.info("Role %d: %d", i, static_cast<uint8_t>(roles[i]));
    }
    if (roles[0] == DeviceRole::Master)
    {
      masterTask.start({STACK_MASTER, PRIORITY_MASTER, CORE_MASTER});
    }
    if (roles[0] == DeviceRole::Keyboard)
    {
      slaveTask.start({STACK_SLAVE, PRIORITY_SLAVE, CORE_SLAVE});
      keyScannerTask.start({STACK_KEYSCAN, PRIORITY_KEYSCAN, CORE_KEYSCAN});
    }
  }

private:
  Platform &platform;
  ConfigManager configManager;

  // Task classes are stored here
  EventBusTask eventBusTask;
  LoggerTask loggerTask;
  KeyScannerTask keyScannerTask;
  MasterTask masterTask;
  SlaveTask slaveTask;
};

#endif