#include <system/TaskManager.h>

static Logger taskLog(TaskManager::NAMESPACE);

static inline uint32_t getRequiredTasksForAllModules(DeviceModule modules[(size_t)DeviceModule::Count]);
static inline uint32_t getRequiredTaskForModule(DeviceModule module);

void TaskManager::start()
{
    configManager.createConfig<GlobalConfig>();
    configManager.createConfig<KeyScannerConfig>();
    startModules(getAllRequiredTasks());
}

ConfigManager *TaskManager::getConfigManagerPointer()
{
    return &configManager;
}

void TaskManager::startModules(uint32_t moduleBitmap)
{
    uint32_t toStop = currentTasks & ~moduleBitmap;
    uint32_t toRestart = currentTasks & moduleBitmap;
    uint32_t toStart = moduleBitmap & ~currentTasks;

    for (uint32_t bit = 1; bit != 0; bit <<= 1)
    {
        if (toStop & bit)
            stopTaskByBit(bit);
        if (toRestart & bit)
            restartTaskByBit(bit);
        if (toStart & bit)
            startTaskByBit(bit);
    }
}

void TaskManager::stopTaskByBit(uint32_t bit)
{
    switch (bit)
    {
    case TaskId::LOGGER_TASK:
        taskLog.info("Stopping LoggerTask");
        loggerTask.stop();
        currentTasks &= ~bit;
        break;
    case TaskId::EVENT_BUS_TASK:
        taskLog.info("Stopping EventBusTask");
        eventBusTask.stop();
        currentTasks &= ~bit;
        break;
    case TaskId::MASTER_TASK:
        taskLog.info("Stopping MasterTask");
        masterTask.stop();
        currentTasks &= ~bit;
        break;
    case TaskId::SLAVE_TASK:
        taskLog.info("Stopping SlaveTask");
        slaveTask.stop();
        currentTasks &= ~bit;
        break;
    case TaskId::KEYSCANNER_TASK:
        taskLog.info("Stopping KeyScannerTask");
        keyScannerTask.stop();
        currentTasks &= ~bit;
        break;
    case TaskId::HIDOUTPUT_TASK:
        taskLog.info("Stopping HidOutputTask");
        hidOutputTask.stop();
        currentTasks &= ~bit;
        break;
    default:
        taskLog.warn("Unknown task bit: %u", bit);
        break;
    }
}

void TaskManager::restartTaskByBit(uint32_t bit)
{
    switch (bit)
    {
    case TaskId::LOGGER_TASK:
        taskLog.info("Restarting LoggerTask");
        loggerTask.restart({STACK_LOGGER, PRIORITY_LOGGER, CORE_LOGGER});
        break;
    case TaskId::EVENT_BUS_TASK:
        taskLog.info("Restarting EventBusTask");
        eventBusTask.restart({STACK_EVENTBUS, PRIORITY_EVENTBUS, CORE_EVENTBUS});
        break;
    case TaskId::MASTER_TASK:
        taskLog.info("Restarting MasterTask");
        masterTask.restart({STACK_MASTER, PRIORITY_MASTER, CORE_MASTER});
        break;
    case TaskId::SLAVE_TASK:
        taskLog.info("Restarting SlaveTask");
        slaveTask.restart({STACK_SLAVE, PRIORITY_SLAVE, CORE_SLAVE});
        break;
    case TaskId::KEYSCANNER_TASK:
        taskLog.info("Restarting KeyScannerTask");
        keyScannerTask.restart({STACK_KEYSCAN, PRIORITY_KEYSCAN, CORE_KEYSCAN});
        break;
    case TaskId::HIDOUTPUT_TASK:
        taskLog.info("Restarting HidOutputTask");
        hidOutputTask.restart({STACK_HIDOUTPUT, PRIORITY_HIDOUTPUT, CORE_HIDOUTPUT});
        break;
    default:
        taskLog.warn("Unknown task bit for restart: %u", bit);
        break;
    }
}

void TaskManager::startTaskByBit(uint32_t bit)
{
    switch (bit)
    {
    case TaskId::LOGGER_TASK:
        taskLog.info("Starting LoggerTask");
        loggerTask.start({STACK_LOGGER, PRIORITY_LOGGER, CORE_LOGGER});
        currentTasks |= bit;
        break;
    case TaskId::EVENT_BUS_TASK:
        taskLog.info("Starting EventBusTask");
        eventBusTask.start({STACK_EVENTBUS, PRIORITY_EVENTBUS, CORE_EVENTBUS});
        currentTasks |= bit;
        break;
    case TaskId::MASTER_TASK:
        taskLog.info("Starting MasterTask");
        masterTask.start({STACK_MASTER, PRIORITY_MASTER, CORE_MASTER});
        currentTasks |= bit;
        break;
    case TaskId::SLAVE_TASK:
        taskLog.info("Starting SlaveTask");
        slaveTask.start({STACK_SLAVE, PRIORITY_SLAVE, CORE_SLAVE});
        currentTasks |= bit;
        break;
    case TaskId::KEYSCANNER_TASK:
        taskLog.info("Starting KeyScannerTask");
        keyScannerTask.start({STACK_KEYSCAN, PRIORITY_KEYSCAN, CORE_KEYSCAN});
        currentTasks |= bit;
        break;
    case TaskId::HIDOUTPUT_TASK:
        taskLog.info("Starting HidOutputTask");
        hidOutputTask.start({STACK_HIDOUTPUT, PRIORITY_HIDOUTPUT, CORE_HIDOUTPUT});
        currentTasks |= bit;
        break;
    default:
        taskLog.warn("Unknown task bit: %u", bit);
        break;
    }
}

uint32_t TaskManager::getAllRequiredTasks()
{
    uint32_t bitmap = 0;

    bitmap |= coreModules;

    bool configLoaded = configManager.loadConfigs();
    if (!configLoaded)
    {
        taskLog.warn("Failed to load configuration, using defaults");
    }

    GlobalConfig *globalCfg = configManager.getConfig<GlobalConfig>();
    if (globalCfg == nullptr)
    {
        taskLog.error("Failed to retrieve global config, aborting");
        return 0;
    }

    if (globalCfg->getDeviceMode() == DeviceMode::Master)
    {
        bitmap |= TaskId::MASTER_TASK;
        taskLog.info("Device mode: Master");
    }
    else
    {
        bitmap |= TaskId::SLAVE_TASK;
        taskLog.info("Device mode: Slave");
    }

    DeviceModule modules[(size_t)DeviceModule::Count] = {};
    globalCfg->getDeviceModules(modules, sizeof(modules));

    bitmap |= getRequiredTasksForAllModules(modules);

    return bitmap;
}

uint32_t getRequiredTasksForAllModules(DeviceModule modules[(size_t)DeviceModule::Count])
{
    uint32_t bitmap = 0;
    for (size_t i = 0; i < (size_t)DeviceModule::Count; i++)
    {
        bitmap |= getRequiredTaskForModule(modules[i]);
    }
    return bitmap;
}

uint32_t getRequiredTaskForModule(DeviceModule module)
{
    switch (module)
    {
    case DeviceModule::Keyscanner:
        taskLog.info("Module: Keyscanner");
        return TaskManager::TaskId::KEYSCANNER_TASK;
    case DeviceModule::HidOutput:
        taskLog.info("Module: HidOutput");
        return TaskManager::TaskId::HIDOUTPUT_TASK;
    default:
        return 0;
    }
}
