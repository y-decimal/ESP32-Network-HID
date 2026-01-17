#include <system/TaskManager.h>

TaskManager::TaskManager(ConfigManager &config, IGpio &gpio, IEspNow &espNow)
    : configManager(config), gpio(gpio), espNow(espNow) {}

void TaskManager::start() {

  // Create queues and ensure they exist
  eventBusQueue = xQueueCreate(32, sizeof(Event));
  configASSERT(eventBusQueue != NULL);
  keyEventQueue = xQueueCreate(32, sizeof(KeyEvent) + sizeof(BitMapEvent));
  configASSERT(keyEventQueue != NULL);

  BaseType_t taskCreated = xTaskCreatePinnedToCore(
      taskManagerTask, "taskManager", STACK_TASKMANAGER, this,
      PRIORITY_TASKMANAGER, nullptr, CORE_TASKMANAGER);
  configASSERT(taskCreated == pdPASS);
}

void TaskManager::initializeTasks() {

  // Implement role based logic here later
  startEventBus();
  startKeyScanner(gpio);
  startSlaveEspTask(espNow);
}
