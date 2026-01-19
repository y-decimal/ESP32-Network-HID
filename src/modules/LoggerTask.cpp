#include <modules/LoggerTask.h>

static constexpr const char *LOG_NAMESPACE = "LoggerTask";

struct LogEvent
{
    const char *logNsPointer;
    Logger::LogLevel level;
    const char *logMsgPointer;
};

LoggerTask::LoggerTask(ILogSink &logSink) : logSink(logSink)
{
    if (instance != nullptr)
        return;
    instance = this;
    localQueue = xQueueCreate(32, sizeof(LogEvent));
}

LoggerTask::~LoggerTask()
{
    if (localQueue != nullptr)
    {
        vQueueDelete(localQueue);
        localQueue = nullptr;
    }
    instance = nullptr;
}

void LoggerTask::callback(const char *logNamespace, Logger::LogLevel level, const char *message)
{
    if (!instance || !instance->localQueue)
    {
        Logger::writeWithNamespace(LOG_NAMESPACE, Logger::LogLevel::warn, "Log queue not initialized");
        return;
    }

    LogEvent logEvent{logNamespace, level, message};
    if (xQueueSend(instance->localQueue, &logEvent, 0) != pdPASS)
    {
        Logger::writeWithNamespace(LOG_NAMESPACE, Logger::LogLevel::error, "Failed to send log event to queue");
    }
}

void LoggerTask::taskEntry(void *arg)
{
    LoggerTask *instance = static_cast<LoggerTask *>(arg);
    if (instance->localQueue == nullptr)
    {
        // Failed to create log queue; terminate this task to avoid using a null queue handle.
        Logger::writeWithNamespace(LOG_NAMESPACE, Logger::LogLevel::error, "Failed to create log queue");
        vTaskDelete(nullptr);
    }
    Logger::setLogCallback(callback);

    for (;;)
    {
        LogEvent evt;
        if (xQueueReceive(instance->localQueue, &evt, portMAX_DELAY))
        {
            Logger::writeWithNamespace(evt.logNsPointer, evt.level, evt.logMsgPointer);
        }
    }
}

void LoggerTask::start(TaskParameters params)
{
    if (loggerHandle != nullptr)
        return;

    BaseType_t result = xTaskCreatePinnedToCore(
        LoggerTask::taskEntry, "LoggerTask", params.stackSize,
        this, params.priority, &loggerHandle,
        params.coreAffinity);

    if (result != pdPASS)
    {
        loggerHandle = nullptr;
    }
}

void LoggerTask::stop()
{
    if (loggerHandle == nullptr)
        return;
    vTaskDelete(loggerHandle);
    loggerHandle = nullptr;
}

void LoggerTask::restart(TaskParameters params)
{
    if (loggerHandle != nullptr)
        stop();
    start(params);
}