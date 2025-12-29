#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <FreeRTOS.h>
#include <submodules/ConfigManager.h>
#include <submodules/EventRegistry.h>
#include <system/SystemConfig.h>
#include <system/TaskParameters.h>

class TaskManager {
public:
  TaskManager(ConfigManager &config);

  void start(); // starts the TaskManager task itself

  void notifyConfigChanged() { applyConfigChanges(); } // called by event handler

private:
  // === Internal task entry points ===
  static void keyScannerTask(void *arg);
  static void bitMapSenderTask(void *arg);
  static void priorityEventTask(void *arg);
  static void taskManagerTask(void *arg); // the supervisor loop

  // === Lifecycle helpers ===
  void startKeyScanner();
  void stopKeyScanner();
  void restartKeyScanner();

  void startBitmapSender();
  void stopBitmapSender();
  void restartBitmapSender();

  void startPriorityEventHandler();
  void stopPriorityEventHandler();
  void restartPriorityEventHandler();

  // === Internal helpers ===
  void initializeTasks(); // initializes tasks depending on the role
  void applyConfigChanges();
  void checkTaskHealth();

  // === State ===
  QueueHandle_t highPrioEventQueue = nullptr;
  QueueHandle_t lowPrioEventQueue = nullptr;

  ConfigManager &configManager;

  TaskHandle_t keyScannerHandle = nullptr;
  TaskHandle_t priorityEventHandle = nullptr;
  TaskHandle_t eventHandle = nullptr;
};

#endif