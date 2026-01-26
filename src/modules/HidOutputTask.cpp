#include <modules/HidOutputTask.h>
#include <submodules/Logger.h>

static Logger log(HidOutputTask::NAMESPACE);

HidOutputTask *HidOutputTask::instance = nullptr;

HidOutputTask::HidOutputTask(IHidOutput &hidOut)
{
    this->hidOut = &hidOut;
    instance = this;
    EventRegistry::registerHandler(EventType::HidBitmap, processEvent);
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
            free(hidEvt.bitMapData);
        }
        else
            log.warn("Hid Event could not be read from queue");
    }
}

void HidOutputTask::processEvent(const Event &event)
{
    if (instance == nullptr || instance->localQueue == nullptr)
    {
        log.error("HidOutputTask instance or local queue is not initialized");
        return;
    }

    if (event.type != EventType::HidBitmap)
    {
        log.warn("Received unsupported event type: %d", static_cast<uint8_t>(event.type));
        return;
    }

    Event bitmapEvent = event;

    if (bitmapEvent.hidBitmapEvt.bitmapSize == 0 || bitmapEvent.hidBitmapEvt.bitMapData == nullptr)
    {
        log.error("HidBitmapEvent has invalid size or data");
        return;
    }

    uint8_t *hidBitmapData = (uint8_t *)malloc(bitmapEvent.hidBitmapEvt.bitmapSize);
    uint8_t hidBitmapSize = bitmapEvent.hidBitmapEvt.bitmapSize;
    memcpy(hidBitmapData, bitmapEvent.hidBitmapEvt.bitMapData, bitmapEvent.hidBitmapEvt.bitmapSize);
    bitmapEvent.cleanup(&bitmapEvent);

    HidBitmapEvent hidEvt{hidBitmapSize, hidBitmapData};

    BaseType_t result = xQueueSend(instance->localQueue, &hidEvt, 0);
    if (result != pdPASS)
    {
        log.error("Failed to enqueue HidBitmapEvent");
    }
    else
    {
        log.debug("HidBitmapEvent enqueued successfully: size %zu", hidEvt.bitmapSize);
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

    if (!initialized)
    {
        if (!hidOut->initialize())
        {
            log.error("Hid output interface initialization failed, aborting task creation");
            return;
        }
        initialized = true;
    }

    if (localQueue == nullptr)
    {
        log.info("Creating queue");
        localQueue = xQueueCreate(32, sizeof(HidBitmapEvent));
        if (localQueue == nullptr)
        {
            log.error("Failed to create queue, aborting task creation");
            return;
        }
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