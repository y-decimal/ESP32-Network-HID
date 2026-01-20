#include <modules/LoggerTask.h>

static Logger internalLogInstance(LOGGERTASK_NAMESPACE);

// Initialize static member variable
LoggerTask *LoggerTask::instance = nullptr;

struct LogEvent
{
    const char *logNsPointer;
    Logger::LogLevel level;
    const char *logMsgPointer;
};

LoggerTask::LoggerTask(ILogSink &logSink) : logSink(logSink)
{
    Logger::setGlobalSink(&logSink);
    if (instance != nullptr)
        instance->~LoggerTask();
    instance = this;
    localQueue = xQueueCreate(32, sizeof(LogEvent));
    Logger::setLogCallback(callback); // Set callback after queue creation for safety
}

LoggerTask::~LoggerTask()
{
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
        internalLogInstance.warn("Log queue not initialized");
        return;
    }

    LogEvent logEvent{logNamespace, level, message};
    if (xQueueSend(instance->localQueue, &logEvent, 0) != pdPASS)
    {
        internalLogInstance.error("Failed to send log event to queue");
    }
}

void LoggerTask::taskEntry(void *arg)
{
    LoggerTask *instance = static_cast<LoggerTask *>(arg);
    if (instance->localQueue == nullptr)
    {
        // Failed to create log queue; terminate this task to avoid using a null queue handle.
        internalLogInstance.error("Failed to create log queue");
        vTaskDelete(nullptr);
    }

    for (;;)
    {
        LogEvent evt;
        if (xQueueReceive(instance->localQueue, &evt, portMAX_DELAY))
        {
            internalLogInstance.log(evt.logNsPointer, evt.level, evt.logMsgPointer);
        }
    }
}

void LoggerTask::start(TaskParameters params)
{
    internalLogInstance.info("Starting LoggerTask");
    if (loggerHandle != nullptr)
    {
        internalLogInstance.warn("LoggerTask already running");
        return;
    }
    BaseType_t result = xTaskCreatePinnedToCore(
        LoggerTask::taskEntry, "LoggerTask", params.stackSize,
        this, params.priority, &loggerHandle,
        params.coreAffinity);

    if (result != pdPASS)
    {
        internalLogInstance.error("Failed to create LoggerTask");
        loggerHandle = nullptr;
    }
}

void LoggerTask::stop()
{
    internalLogInstance.info("Stopping LoggerTask");
    if (loggerHandle == nullptr)
    {
        internalLogInstance.info("Stop called but LoggerTask is not running");
        return;
    }
    vTaskDelete(loggerHandle);
    loggerHandle = nullptr;

}

void LoggerTask::restart(TaskParameters params)
{
    internalLogInstance.info("Restarting LoggerTask");
    if (loggerHandle != nullptr)
    {
        stop();
    }
    start(params);
}