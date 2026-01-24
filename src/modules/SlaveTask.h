#ifndef SLAVETASK_H
#define SLAVETASK_H

#include <interfaces/ITask.h>
#include <interfaces/ITransport.h>
#include <submodules/TransportProtocol.h>
#include <submodules/EventRegistry.h>
#include <queue.h>

class SlaveTask : public ITask
{
public:
    static constexpr const char *NAMESPACE = "SlaveTask";

    SlaveTask(ITransport &transport, ConfigManager *config);
    ~SlaveTask();
    void start(TaskParameters params) override;
    void stop() override;
    void restart(TaskParameters params) override;

private:
    TaskHandle_t slaveTaskHandle = nullptr;
    QueueHandle_t localQueue = nullptr;
    ITransport *transportRef = nullptr;
    TransportProtocol *protocol = nullptr;
    ConfigManager *configManager = nullptr;
    static SlaveTask *instance;

    bool connected = false;

    static void taskEntry(void *arg);
    static void eventBusCallback(const Event &evt);
    static void pairConfirmCallback(uint8_t sourceId);
    static void configReceiveCallback(ConfigManager *config, uint8_t senderId);
    static void configRequestCallback(uint8_t senderId);
};

#endif