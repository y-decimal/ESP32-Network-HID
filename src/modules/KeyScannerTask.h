#ifndef KEYSCANNERTASK_H
#define KEYSCANNERTASK_H

#include <interfaces/ITask.h>
#include <submodules/KeyScanner.h>
#include <submodules/Config/ConfigManager.h>
#include <submodules/Config/KeyScannerConfig.h>
#include <submodules/EventRegistry.h>
#include <queue.h>

class KeyScannerTask : public ITask
{
public:
    static constexpr const char *NAMESPACE = "KeyScannerTask";

    KeyScannerTask(ConfigManager *configManager, IGpio &gpio);
    ~KeyScannerTask();
    void start(TaskParameters params) override;
    void stop() override;
    void restart(TaskParameters params) override;

private:
    ConfigManager *configManager = nullptr;
    IGpio *gpioRef = nullptr;
    TaskHandle_t keyScannerTaskHandle = nullptr;
    static KeyScannerTask *instance;

    static void taskEntry(void *param);
    static void keyEventCallback(uint16_t keyIndex, bool state);
    static void sendBitMapEvent(uint8_t bitmapSize, uint8_t *bitMap);
};

#endif