#include <system/TaskManager.h>

TaskManager::TaskManager(ConfigManager &config, IGpio &gpio, ITransport &espNow)
    : configManager(config), gpio(gpio), espNow(espNow) {}

void TaskManager::start()
{

  // Create queues and ensure they exist
  eventBusQueue = xQueueCreate(32, sizeof(Event));
  configASSERT(eventBusQueue != NULL);
  keyEventQueue = xQueueCreate(32, sizeof(Event));
  configASSERT(keyEventQueue != NULL);

  BaseType_t taskCreated = xTaskCreatePinnedToCore(
      taskManagerTask, "taskManager", STACK_TASKMANAGER, this,
      PRIORITY_TASKMANAGER, nullptr, CORE_TASKMANAGER);
  configASSERT(taskCreated == pdPASS);
}

void TaskManager::initializeTasks()
{

  // Implement role based logic here later
  startEventBus();
  startLogger();
  
  DeviceRole roles[static_cast<size_t>(DeviceRole::Count)] = {DeviceRole::Count};
  GlobalConfig gCfg = configManager.getConfig<GlobalConfig>();
  gCfg.getRoles(roles, static_cast<size_t>(DeviceRole::Count));
  if (roles[0] == DeviceRole::Master)
  {
    logger.info("Starting Master ESP Task");
    stopSlaveEspTask();
    stopKeyScanner();
    startMasterEspTask(espNow);
  }
  else if (roles[0] == DeviceRole::Keyboard)
  {
    logger.info("Starting Slave ESP Task and KeyScanner Task");
    stopMasterEspTask();
    startKeyScanner(gpio);
    startSlaveEspTask(espNow);
  }
  else
  {
    logger.info("No ESP Task started due to undefined role");
    stopMasterEspTask();
    stopSlaveEspTask();
    stopKeyScanner();
  }
}
