#include <system/TaskManager.h>
#include <submodules/Logger.h>

static QueueHandle_t localLogQueueReference = nullptr;

struct LogEvent
{
    const char *logNsPointer;
    Logger::LogLevel level;
    const char *logMsgPointer;
};

void callback(const char *logNamespace, Logger::LogLevel level, const char *message)
{
    if (!localLogQueueReference)
        return;

    LogEvent logEvent{logNamespace, level, message};
    xQueueSend(localLogQueueReference, &logEvent, pdMS_TO_TICKS(15));
}

void TaskManager::loggerTask(void *arg)
{
    localLogQueueReference = xQueueCreate(32, sizeof(LogEvent));
    Logger::setLogCallback(callback);

    for (;;)
    {
        LogEvent evt;
        if (xQueueReceive(localLogQueueReference, &evt, portMAX_DELAY))
        {
            Logger::writeWithNamespace(evt.logNsPointer, evt.level, evt.logMsgPointer);
        }
    }
}

void TaskManager::startLogger()
{
    if (loggerHandle != nullptr)
        return;

    BaseType_t result = xTaskCreatePinnedToCore(
        loggerTask, "LoggerTask", STACK_LOGGER,
        nullptr, PRIORITY_LOGGER, &loggerHandle,
        CORE_LOGGER);

    if (result != pdPASS)
    {
        loggerHandle = nullptr;
    }
}

void TaskManager::stopLogger()
{
    if (loggerHandle == nullptr)
        return;
    vTaskDelete(loggerHandle);
    loggerHandle = nullptr;
}

void TaskManager::restartLogger()
{
    if (loggerHandle != nullptr)
        stopLogger();
    startLogger();
}