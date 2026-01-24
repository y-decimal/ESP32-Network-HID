#ifndef LOGGERTASK_H
#define LOGGERTASK_H

#include <interfaces/ITask.h>
#include <queue.h>
#include <submodules/Logger.h>

class LoggerTask : public ITask
{
public:
    static constexpr const char *NAMESPACE = "LoggerTask";

    LoggerTask();
    ~LoggerTask();
    void start(TaskParameters params) override;
    void stop() override;
    void restart(TaskParameters params) override;

private:
    QueueHandle_t localQueue;
    TaskHandle_t loggerHandle = nullptr;
    static LoggerTask *instance;

    static void taskEntry(void *param);

    static void callback(const char *logNamespace, Logger::LogLevel level, const char *message);
};

#endif