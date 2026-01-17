#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <FreeRTOS.h>
#include <interfaces/IEspNow.h>
#include <interfaces/IGpio.h>
#include <submodules/ConfigManager/ConfigManager.h>
#include <submodules/EventRegistry.h>
#include <system/SystemConfig.h>
#include <system/TaskParameters.h>

/**
 * @brief Central controller for the lifecycle of system tasks.
 *
 * TaskManager is responsible for creating, starting, stopping, and
 * supervising the core FreeRTOS tasks in the system (such as key
 * scanning, bitmap sending, and priority event handling). It owns
 * the associated task handles and event queues and applies runtime
 * configuration changes received from ConfigManager.
 *
 * Unless otherwise documented, public methods are expected to be
 * called from the normal task context (not from an ISR). The
 * TaskManager itself typically runs a supervisor loop (see
 * taskManagerTask) that reacts to configuration updates and task
 * health checks.
 */
class TaskManager {
public:
  /**
   * @brief Construct a new TaskManager.
   *
   * @param config Reference to the global ConfigManager used to
   *               initialize and update task parameters. The
   *               referenced ConfigManager instance must outlive
   *               this TaskManager.
   */
  TaskManager(ConfigManager &config, IGpio &gpio, IEspNow &espNow);

  /**
   * @brief Start the TaskManager supervisor task and any managed tasks.
   *
   * This method should be called once during system startup, after
   * the scheduler and core infrastructure have been initialized but
   * before normal application operation begins. It creates and/or
   * resumes the internal FreeRTOS tasks that TaskManager controls.
   */
  void start();

  /**
   * @brief Notify TaskManager that configuration has changed.
   *
   * This is typically invoked by a configuration or event handler
   * when system settings that affect task behavior are updated.
   * The notification causes TaskManager's supervisor logic to
   * re-read configuration via ConfigManager and apply any required
   * changes to managed tasks (for example, restarting tasks with
   * new parameters).
   *
   * Threading note: This function is intended to be called from
   * task context. If called from an ISR, the caller must ensure
   * that the underlying implementation is ISR-safe or provide an
   * appropriate deferred call mechanism.
   */
  void notifyConfigChanged();

private:
  // === Internal task entry points ===
  static void keyScannerTask(void *arg);
  static void eventBusTask(void *arg);
  static void slaveEspTask(void *arg);
  static void taskManagerTask(void *arg); // the supervisor loop

  // === Lifecycle helpers ===
  void startKeyScanner(IGpio &gpio);
  void stopKeyScanner();
  void restartKeyScanner(IGpio &gpio);

  void startEventBus();
  void stopEventBus();
  void restartEventBus();

  void startSlaveEspTask(IEspNow &espNow);
  void stopSlaveEspTask();
  void restartSlaveEspTask(IEspNow &espNow);

  // === Internal helpers ===
  void initializeTasks();    // initializes tasks depending on the role
  void applyConfigChanges(); // TODO: Implement config-dependent task adjustment
                             // logic.
  void checkTaskHealth(); // TODO: Implement task health monitoring and recovery
                          // logic.

  // === State ===
  QueueHandle_t eventBusQueue = nullptr;
  QueueHandle_t keyEventQueue = nullptr;

  IGpio &gpio;
  IEspNow &espNow;

  // NOTE: This reference may be accessed from multiple FreeRTOS tasks.
  // It is required that either:
  //   - ConfigManager provides its own internal thread-safety for concurrent
  //     access from multiple tasks, or
  //   - All accesses to configManager via TaskManager are externally
  //     synchronized so that no data races occur.
  ConfigManager &configManager;

  TaskHandle_t keyScannerHandle = nullptr;
  TaskHandle_t eventBusHandle = nullptr;
  TaskHandle_t slaveEspHandle = nullptr;
};

#endif