#include <system/TaskManager.h>

TaskManager::TaskManager(ConfigManager &config) : configManager(config) {}

void TaskManager::start() {

  // Create queues and ensure they exist
  highPrioEventQueue = xQueueCreate(32, sizeof(Event));
  configASSERT(highPrioEventQueue != NULL);
  lowPrioEventQueue = xQueueCreate(32, sizeof(Event));
  configASSERT(lowPrioEventQueue != NULL);

  // Initialize scannerState with parameters from config
  uint8_t rows = configManager.getConfig<KeyScannerConfig>().rows;
  uint8_t cols = configManager.getConfig<KeyScannerConfig>().cols;

  DeviceRole roles[(size_t)DeviceRole::Count];
  memcpy(roles, configManager.getConfig<GlobalConfig>().roles, sizeof(roles));

  // EventHandler always runs
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

void TaskManager::notifyConfigChanged() {}
