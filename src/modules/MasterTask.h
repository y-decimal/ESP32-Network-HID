#ifndef MASTERTASK_H
#define MASTERTASK_H

#include <interfaces/ITask.h>
#include <interfaces/ITransport.h>
#include <submodules/TransportProtocol.h>
#include <submodules/EventRegistry.h>
#include <submodules/Logger.h>

static constexpr const char *MASTERTASK_NAMESPACE = "MasterTask";

class MasterTask : public ITask
{
public:
    MasterTask(ITransport &transport);
    ~MasterTask();
    void start(TaskParameters params) override;
    void stop() override;
    void restart(TaskParameters params) override;

private:
    TaskHandle_t masterTaskHandle = nullptr;
    ITransport *transportRef = nullptr;
    Logger *internalLog = nullptr;
    TransportProtocol *protocol = nullptr;
    static MasterTask *instance;

    static void taskEntry(void *arg);
    static void pairReceiveCallback(const uint8_t *data, uint8_t sourceId);
    static void keyReceiveCallback(const RawKeyEvent &keyEvent, uint8_t senderId);
    static void bitmapReceiveCallback(const RawBitmapEvent &bitmapEvent, uint8_t senderId);
};

#endif