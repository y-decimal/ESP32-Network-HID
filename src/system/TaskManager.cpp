#include <system/TaskManager.h>

TaskManager::TaskManager(ConfigManager &config, IGpio &gpio, IEspNow &espNow)
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
  startKeyScanner(gpio);

  DeviceRole roles[static_cast<size_t>(DeviceRole::Count)] = {DeviceRole::Count};
  GlobalConfig gCfg = configManager.getConfig<GlobalConfig>();
  gCfg.getRoles(roles, static_cast<size_t>(DeviceRole::Count));
  if (roles[0] == DeviceRole::Master)
  {
    printf("Starting Master ESP Task\n");
    stopSlaveEspTask();
    stopKeyScanner();
    startMasterEspTask(espNow);
  }
  else if (roles[0] == DeviceRole::Keyboard)
  {
    printf("Starting Slave ESP Task\n");
    stopMasterEspTask();
    startKeyScanner(gpio);
    startSlaveEspTask(espNow);
  }
  else
  {
    printf("No ESP Task started\n");
    stopMasterEspTask();
    stopSlaveEspTask();
    stopKeyScanner();
  }
}
