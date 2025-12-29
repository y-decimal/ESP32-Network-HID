#include <system/TaskManager.h>

TaskManager::TaskManager(ConfigManager &config) : configManager(config) {}

void TaskManager::start() {

  // Create queues and ensure they exist
  highPrioEventQueue = xQueueCreate(32, sizeof(Event));
  configASSERT(highPrioEventQueue != NULL);
  lowPrioEventQueue = xQueueCreate(32, sizeof(Event));
  configASSERT(lowPrioEventQueue != NULL);

  initializeTasks();
}

void TaskManager::initializeTasks() {
  // Retrieve roles from config
  DeviceRole roles[(size_t)DeviceRole::Count];
  memcpy(roles, configManager.getConfig<GlobalConfig>().roles, sizeof(roles));

  // EventHandler always runs
  registerEventCallbacks();
  startEventHandler();

  for (size_t i = 0; i < sizeof(roles); i++) {
    switch (roles[i]) {
    case DeviceRole::Keyboard:
      startKeyScanner();
      break;
    case DeviceRole::Master:
      // start master modules here
      break;
    default:
      break;
    }
  }
}
