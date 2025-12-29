#include <system/TaskManager.h>

TaskManager::TaskManager(ConfigManager &config) : configManager(config) {}

void TaskManager::start() {

  // Create queues and ensure they exist
  highPrioEventQueue = xQueueCreate(32, sizeof(Event));
  configASSERT(highPrioEventQueue != NULL);
  lowPrioEventQueue = xQueueCreate(32, sizeof(Event));
  configASSERT(lowPrioEventQueue != NULL);

  BaseType_t taskCreated = xTaskCreatePinnedToCore(
      taskManagerTask, "taskManager", STACK_TASKMANAGER,
      this, PRIORITY_TASKMANAGER, nullptr, CORE_TASKMANAGER);
  configASSERT(taskCreated == pdPASS);
}

void TaskManager::initializeTasks() {
  // Retrieve roles from config
  DeviceRole roles[(size_t)DeviceRole::Count];
  memcpy(roles, configManager.getConfig<GlobalConfig>().roles, sizeof(roles));

  // EventHandler always runs
  startPriorityEventHandler();
  startKeyScanner();
}
