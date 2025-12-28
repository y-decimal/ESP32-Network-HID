#include <submodules/ConfigManager.h>

GlobalConfig ConfigManager::getGlobalConfig() const { return globalCfg.get(); }

KeyScannerConfig ConfigManager::getKeyConfig() const {
  return keyScannerCfg.get();
}

void ConfigManager::setGlobalConfig(GlobalConfig cfg) { globalCfg.set(cfg); }

void ConfigManager::setKeyConfig(KeyScannerConfig cfg) {
  keyScannerCfg.set(cfg);
}

bool ConfigManager::saveConfig() {

  bool globalSaved = false;
  bool keySaved = false;

  if (globalCfg.isDirty())
    globalSaved = globalCfg.save();
  if (keyScannerCfg.isDirty())
    keySaved = keyScannerCfg.save();

  return globalSaved && keySaved;
}

bool ConfigManager::loadConfig() {

  bool globalLoaded = false;
  bool keyLoaded = false;

  if (!globalCfg.isDirty())
    globalLoaded = globalCfg.load();
  if (!keyScannerCfg.isDirty())
    keyLoaded = keyScannerCfg.load();

  return globalLoaded && keyLoaded;
}