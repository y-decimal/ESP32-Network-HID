#include <submodules/Config/ConfigManager.h>

bool ConfigManager::saveConfig()
{
  bool globalSaved = globalCfg.save();
  bool keySaved = keyScannerCfg.save();

  if (!globalSaved)
    configLog.error("Failed to save GlobalConfig");
  if (!keySaved)
    configLog.error("Failed to save KeyScannerConfig");

  configLog.info("Configuration save status: GlobalConfig=%s, KeyScannerConfig=%s",
                 globalSaved ? "success" : "failure",
                 keySaved ? "success" : "failure");

  return globalSaved && keySaved;
}

bool ConfigManager::loadConfig()
{
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

bool ConfigManager::clearAllConfigs()
{
  bool globalCleared = globalCfg.clearAll();
  bool keyCleared = keyScannerCfg.clearAll();

  return globalCleared && keyCleared;
}