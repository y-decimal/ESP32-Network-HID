#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <FreeRTOS.h>
#include <shared/DataTypes.h>
#include <submodules/ConfigManager.h>
#include <submodules/EventRegistry.h>
#include <system/SystemConfig.h>
#include <system/TaskParameters.h>

class TaskManager {
public:
  TaskManager(ConfigManager &config);

  void start();           // starts the TaskManager task itself
  void initializeTasks(); // initializes tasks depending on the role

  void notifyConfigChanged(); // called by event handler

private:
  // === Internal task entry points ===
  static void keyScannerTask(void *arg);
  static void bitMapSenderTask(void *arg);
  static void eventHandlerTask(void *arg);
  static void taskManagerTask(void *arg); // the supervisor loop

  // === Lifecycle helpers ===
  void startKeyScanner();
  void stopKeyScanner();
  void restartKeyScanner();

  void startBitmapSender();
  void stopBitmapSender();
  void restartBitmapSender();

  void startEventHandler();
  void stopEventHandler();
  void restartEventHandler();

  // === Internal helpers ===
  void applyConfigChanges();
  void checkTaskHealth();
  IBitMapRoutingStrategy *createRoutingStrategy();

  // === State ===
  QueueHandle_t highPrioEventQueue = nullptr;
  QueueHandle_t lowPrioEventQueue = nullptr;

  ConfigManager &configManager;

  TaskHandle_t keyScannerHandle = nullptr;
  TaskHandle_t bitmapSenderHandle = nullptr;
  TaskHandle_t eventManagerHandle = nullptr;
  TaskHandle_t managerHandle = nullptr;

  KeyScannerState *keyScannerState = nullptr;
};

#endif