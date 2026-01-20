#ifndef KEYSCANNERTASK_H
#define KEYSCANNERTASK_H

#include <interfaces/ITask.h>
#include <submodules/KeyScanner.h>
#include <submodules/Config/ConfigManager.h>
#include <submodules/EventRegistry.h>
#include <queue.h>

static constexpr const char *KEYSCANNER_NAMESPACE = "KeyScannerTask";

class KeyScannerTask : public ITask
{
public:
    KeyScannerTask(ConfigManager &configManager, IGpio &gpio);
    ~KeyScannerTask();
    void start(TaskParameters params) override;
    void stop() override;
    void restart(TaskParameters params) override;

private:
    ConfigManager *configManagerRef = nullptr;
    IGpio *gpioRef = nullptr;
    TaskHandle_t keyScannerTaskHandle = nullptr;
    static KeyScannerTask *instance;

    static void taskEntry(void *param);
    static void keyEventCallback(uint16_t keyIndex, bool state);
    static void sendBitMapEvent(uint8_t bitMapSize, uint8_t *bitMap);
};

#endif