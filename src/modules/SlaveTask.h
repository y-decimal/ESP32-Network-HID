#ifndef SLAVETASK_H
#define SLAVETASK_H

#include <interfaces/ITask.h>
#include <interfaces/ITransport.h>
#include <submodules/TransportProtocol.h>
#include <submodules/EventRegistry.h>
#include <queue.h>

static constexpr const char *SLAVETASK_NAMESPACE = "SlaveTask";

class SlaveTask : public ITask
{
public:
    SlaveTask(ITransport &transport);
    ~SlaveTask();
    void start(TaskParameters params) override;
    void stop() override;
    void restart(TaskParameters params) override;

private:
    TaskHandle_t slaveTaskHandle = nullptr;
    QueueHandle_t localQueue = nullptr;
    ITransport *transportRef = nullptr;
    TransportProtocol *protocol = nullptr;
    static SlaveTask *instance;

    bool connected = false;

    static void taskEntry(void *arg);
    static void eventBusCallback(const Event &evt);
    static void pairConfirmCallback(uint8_t sourceId);
    static void configReceiveCallback(const ConfigManager &config, uint8_t senderId);
};

#endif