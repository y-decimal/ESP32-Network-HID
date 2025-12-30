#include <submodules/ConfigManager/ConfigManager.h>

bool ConfigManager::saveConfig() {

  bool globalSaved = false;
  bool keySaved = false;

  if (globalCfg.isDirty())
    globalSaved = globalCfg.save();
  else
    globalSaved = true;

  if (keyScannerCfg.isDirty())
    keySaved = keyScannerCfg.save();
  else
    keySaved = true;

  return globalSaved && keySaved;
}

bool ConfigManager::loadConfig() {

  bool globalLoaded = false;
  bool keyLoaded = false;

  if (!globalCfg.isDirty())
    globalLoaded = globalCfg.load();
  else
    globalLoaded = true;

  if (!keyScannerCfg.isDirty())
    keyLoaded = keyScannerCfg.load();
  else
    keyLoaded = true;

  return globalLoaded && keyLoaded;
}