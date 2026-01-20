#include <system/TaskManager.h>

static inline uint32_t getRequiredTasksForAllModules(DeviceModule modules[(size_t)DeviceModule::Count]);
static inline uint32_t getRequiredTaskForModule(DeviceModule module);

void TaskManager::start()
{
    loggerTask.start({STACK_LOGGER, PRIORITY_LOGGER, CORE_LOGGER});
    eventBusTask.start({STACK_EVENTBUS, PRIORITY_EVENTBUS, CORE_EVENTBUS});

    startModules(getAllRequiredTasks());
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
    case TaskId::EVENT_BUS_TASK:
        eventBusTask.stop();
        break;
    case TaskId::LOGGER_TASK:
        loggerTask.stop();
        break;
    case TaskId::KEYSCANNER_TASK:
        keyScannerTask.stop();
        break;
    case TaskId::MASTER_TASK:
        masterTask.stop();
        break;
    case TaskId::SLAVE_TASK:
        slaveTask.stop();
        break;
    default:
        break;
    }
}

void TaskManager::restartTaskByBit(uint32_t bit)
{
    switch (bit)
    {
    case TaskId::EVENT_BUS_TASK:
        eventBusTask.restart({STACK_EVENTBUS, PRIORITY_EVENTBUS, CORE_EVENTBUS});
        break;
    case TaskId::LOGGER_TASK:
        loggerTask.restart({STACK_LOGGER, PRIORITY_LOGGER, CORE_LOGGER});
        break;
    case TaskId::KEYSCANNER_TASK:
        keyScannerTask.restart({STACK_KEYSCAN, PRIORITY_KEYSCAN, CORE_KEYSCAN});
        break;
    case TaskId::MASTER_TASK:
        masterTask.restart({STACK_MASTER, PRIORITY_MASTER, CORE_MASTER});
        break;
    case TaskId::SLAVE_TASK:
        slaveTask.restart({STACK_SLAVE, PRIORITY_SLAVE, CORE_SLAVE});
        break;
    default:
        break;
    }
}

void TaskManager::startTaskByBit(uint32_t bit)
{
    switch (bit)
    {
    case TaskId::EVENT_BUS_TASK:
        eventBusTask.start({STACK_EVENTBUS, PRIORITY_EVENTBUS, CORE_EVENTBUS});
        break;
    case TaskId::LOGGER_TASK:
        loggerTask.start({STACK_LOGGER, PRIORITY_LOGGER, CORE_LOGGER});
        break;
    case TaskId::KEYSCANNER_TASK:
        keyScannerTask.start({STACK_KEYSCAN, PRIORITY_KEYSCAN, CORE_KEYSCAN});
        break;
    case TaskId::MASTER_TASK:
        masterTask.start({STACK_MASTER, PRIORITY_MASTER, CORE_MASTER});
        break;
    case TaskId::SLAVE_TASK:
        slaveTask.start({STACK_SLAVE, PRIORITY_SLAVE, CORE_SLAVE});
        break;
    default:
        break;
    }
}

uint32_t TaskManager::getAllRequiredTasks()
{
    uint32_t bitmap = 0;

    bool configLoaded = configManager.loadConfig();
    if (!configLoaded)
        taskLog.warn("Failed to load configuration, using defaults");

    configManager.getConfig<GlobalConfig>().getDeviceMode() == DeviceMode::Master
        ? bitmap |= TaskId::MASTER_TASK
        : bitmap |= TaskId::SLAVE_TASK;

    DeviceModule modules[(size_t)DeviceModule::Count] = {};
    configManager.getConfig<GlobalConfig>().getDeviceModules(modules, sizeof(modules));

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
        return TaskManager::TaskId::KEYSCANNER_TASK;
    default:
        return 0;
    }
}
