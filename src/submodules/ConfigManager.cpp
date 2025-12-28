#include <submodules/ConfigManager.h>

GlobalConfig ConfigManager::getGlobalConfig() const { return globalCfg; }

KeyScannerConfig ConfigManager::getKeyConfig() const { return keyScannerCfg; }