#include <system/TaskManager.h>

void TaskManager::taskManagerTask(void *args) {
  TaskManager *self = static_cast<TaskManager *>(args);

  self->initializeTasks();

  TickType_t previousWakeTime = xTaskGetTickCount();

  while (true) {
    printf("running...\n");
    xTaskDelayUntil(&previousWakeTime, PERIOD_TASKMANAGER);
  }
}