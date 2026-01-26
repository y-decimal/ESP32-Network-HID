#include <modules/HidOutputTask.h>
#include <submodules/Logger.h>
#include <shared/EventTypes.h>

static Logger log(HidOutputTask::NAMESPACE);

HidOutputTask::HidOutputTask(IHidOutput &hidOut)
{
    this->hidOut = &hidOut;
    instance = this;
}

HidOutputTask::~HidOutputTask()
{
    if (instance)
    {
        instance->stop();
        instance = nullptr;
    }
}

void HidOutputTask::taskEntry(void *param)
{
    for (;;)
    {
        HidBitmapEvent hidEvt;
        if (xQueueReceive(instance->localQueue, &hidEvt, portMAX_DELAY))
        {
            instance->hidOut->sendHidReport(hidEvt.bitMapData, hidEvt.bitmapSize);
            log.info("Hid bitmap event sent to output: size %zu, data %d", hidEvt.bitmapSize, hidEvt.bitMapData);
            // Note, with current implementation, cleaning up the bitmap data buffer may not be possible
            // and every hid bitmap event will leak memory gradually. Need to investigate event bus and
            // event registry implementation
        }
        else
            log.warn("Hid Event could not be read from queue");
    }
}

void HidOutputTask::start(TaskParameters params)
{
    log.info("Starting HidOutputTask with stack size %u, priority %d, core affinity %d",
             params.stackSize, params.priority, params.coreAffinity);

    if (hidOutputTaskHandle != nullptr)
    {
        log.warn("HidOutputTask already running, aborting task creation");
        return;
    }

    if (instance == nullptr || instance != this)
    {
        log.error("Instance pointer not set correctly, aborting task creation");
        return;
    }

    if (hidOut == nullptr)
    {
        log.error("Hid output interface not set, aborting task creation");
        return;
    }

    if (localQueue == nullptr)
    {
        log.info("Creating queue");
        xQueueCreate(16, sizeof(HidBitmapEvent));
    }

    BaseType_t result = xTaskCreatePinnedToCore(taskEntry,
                                                NAMESPACE,
                                                params.stackSize,
                                                nullptr,
                                                params.priority,
                                                &hidOutputTaskHandle,
                                                params.coreAffinity);
    if (result != pdPASS)
    {
        log.error("Failed to create HidOutputTask!");
        hidOutputTaskHandle = nullptr;
        vQueueDelete(localQueue);
        localQueue = nullptr;
    }
}

void HidOutputTask::stop()
{
    log.info("Stopping HidOutputTask");
    if (hidOutputTaskHandle == nullptr)
    {
        log.warn("Stop called but HidOutputTask is not running");
        return;
    }

    if (localQueue != nullptr)
    {
        vQueueDelete(localQueue);
        localQueue = nullptr;
    }

    vTaskDelete(hidOutputTaskHandle);
    hidOutputTaskHandle = nullptr;
}

void HidOutputTask::restart(TaskParameters params)
{
    log.info("Restarting HidOutputTask");
    if (hidOutputTaskHandle != nullptr)
        stop();
    start(params);
}