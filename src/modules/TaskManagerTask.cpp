#include <system/TaskManager.h>

void TaskManager::taskManagerTask(void *args) {
  TaskManager *self = static_cast<TaskManager *>(args);

  self->initializeTasks();

  TickType_t previousWakeTime = xTaskGetTickCount();

  uint16_t loopsSincePrint = 0;

  while (true) {
    // loopsSincePrint++;
    // if (loopsSincePrint > 10) {
    //   printf("running...\n");
    //   loopsSincePrint = 0;
    // }
    xTaskDelayUntil(&previousWakeTime, PERIOD_TASKMANAGER);
  }
}