#include <submodules/ConfigManager.h>

GlobalConfig ConfigManager::getGlobalConfig() const { return globalCfg; }

KeyScannerConfig ConfigManager::getKeyConfig() const { return keyScannerCfg; }

void ConfigManager::setGlobalConfig(GlobalConfig cfg) { globalCfg = cfg; }

void ConfigManager::setKeyConfig(KeyScannerConfig cfg) { keyScannerCfg = cfg; }