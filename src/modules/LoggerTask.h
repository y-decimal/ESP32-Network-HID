#ifndef LOGGERTASK_H
#define LOGGERTASK_H

#include <interfaces/ITask.h>
#include <queue.h>
#include <submodules/Logger.h>

static constexpr const char *LOGGERTASK_NAMESPACE = "LoggerTask";

class LoggerTask : public ITask
{
public:
    LoggerTask(ILogSink &logSink);
    ~LoggerTask();
    void start(TaskParameters params) override;
    void stop() override;
    void restart(TaskParameters params) override;

private:
    QueueHandle_t localQueue;
    TaskHandle_t loggerHandle = nullptr;
    Logger *internalLog = nullptr;
    static LoggerTask *instance;
    ILogSink &logSink;

    static void taskEntry(void *param);

    static void callback(const char *logNamespace, Logger::LogLevel level, const char *message);
};

#endif