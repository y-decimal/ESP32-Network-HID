#ifndef HIDOUTPUTTASK_H
#define HIDOUTPUTTASK_H

#include <interfaces/ITask.h>
#include <interfaces/IHidOutput.h>
#include <submodules/EventRegistry.h>
#include <queue.h>

class HidOutputTask : public ITask
{
public:
    static constexpr const char *NAMESPACE = "HidOutputTask";

    HidOutputTask(IHidOutput &hidOut);
    ~HidOutputTask();

    void start(TaskParameters params) override;
    void stop() override;
    void restart(TaskParameters params) override;

private:
    IHidOutput *hidOut = nullptr;
    QueueHandle_t localQueue = nullptr;
    TaskHandle_t hidOutputTaskHandle = nullptr;
    static HidOutputTask *instance;

    bool initialized = false;

    static void taskEntry(void *param);

    static void processEvent(const Event &event);
};

#endif