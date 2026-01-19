#include <modules/LoggerTask.h>

// Initialize static member variable
LoggerTask *LoggerTask::instance = nullptr;

static constexpr const char *MASTERTASK_NAMESPACE = "LoggerTask";

struct LogEvent
{
    const char *logNsPointer;
    Logger::LogLevel level;
    const char *logMsgPointer;
};

LoggerTask::LoggerTask(ILogSink &logSink) : logSink(logSink), internalLog(MASTERTASK_NAMESPACE)
{
    Logger::setGlobalSink(&logSink);
    if (instance != nullptr)
        return;
    instance = this;
    localQueue = xQueueCreate(32, sizeof(LogEvent));
    Logger::setLogCallback(callback); // Set callback after queue creation for safety
}

LoggerTask::~LoggerTask()
{
    internalLog.info("Destroying LoggerTask");
    if (localQueue != nullptr)
    {
        vQueueDelete(localQueue);
        localQueue = nullptr;
    }
    instance = nullptr;
    Logger::setGlobalSink(nullptr);
    Logger::setLogCallback(nullptr);
}

void LoggerTask::callback(const char *logNamespace, Logger::LogLevel level, const char *message)
{
    if (!instance || !instance->localQueue)
    {
        instance->internalLog.warn("Log queue not initialized");
        return;
    }

    LogEvent logEvent{logNamespace, level, message};
    if (xQueueSend(instance->localQueue, &logEvent, 0) != pdPASS)
    {
        instance->internalLog.error("Failed to send log event to queue");
    }
}

void LoggerTask::taskEntry(void *arg)
{
    LoggerTask *instance = static_cast<LoggerTask *>(arg);
    if (instance->localQueue == nullptr)
    {
        // Failed to create log queue; terminate this task to avoid using a null queue handle.
        instance->internalLog.error("Failed to create log queue");
        vTaskDelete(nullptr);
    }

    for (;;)
    {
        LogEvent evt;
        if (xQueueReceive(instance->localQueue, &evt, portMAX_DELAY))
        {
            instance->internalLog.log(evt.logNsPointer, evt.level, evt.logMsgPointer);
        }
    }
}

void LoggerTask::start(TaskParameters params)
{
    internalLog.info("Starting LoggerTask");
    if (loggerHandle != nullptr)
    {
        internalLog.warn("LoggerTask already running");
        return;
    }
    BaseType_t result = xTaskCreatePinnedToCore(
        LoggerTask::taskEntry, "LoggerTask", params.stackSize,
        this, params.priority, &loggerHandle,
        params.coreAffinity);

    if (result != pdPASS)
    {
        internalLog.error("Failed to create LoggerTask");
        loggerHandle = nullptr;
    }
}

void LoggerTask::stop()
{
    internalLog.info("Stopping LoggerTask");
    if (loggerHandle == nullptr)
    {
        internalLog.info("Stop called but LoggerTask is not running");
        return;
    }
    vTaskDelete(loggerHandle);
    loggerHandle = nullptr;
}

void LoggerTask::restart(TaskParameters params)
{
    internalLog.info("Restarting LoggerTask");
    if (loggerHandle != nullptr)
    {
        stop();
    }
    start(params);
}