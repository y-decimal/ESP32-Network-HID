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
  {
    globalLoaded = false;
    configLog.warn("GlobalConfig dirty, skipping load");
  }

  if (!keyScannerCfg.isDirty())
    keyLoaded = keyScannerCfg.load();
  else
  {
    keyLoaded = false;
    configLog.warn("KeyScannerConfig dirty, skipping load");
  }

  if (!globalLoaded)
    configLog.error("Failed to load GlobalConfig");
  if (!keyLoaded)
    configLog.error("Failed to load KeyScannerConfig");

  configLog.info("Configuration load status: GlobalConfig=%s, KeyScannerConfig=%s",
                 globalLoaded ? "success" : "failure",
                 keyLoaded ? "success" : "failure");

  return globalLoaded && keyLoaded;
}

bool ConfigManager::clearAllConfigs()
{
  bool globalCleared = globalCfg.clearAll();
  bool keyCleared = keyScannerCfg.clearAll();
  if (!globalCleared)
    configLog.error("Failed to clear GlobalConfig");
  if (!keyCleared)
    configLog.error("Failed to clear KeyScannerConfig");
  return globalCleared && keyCleared;
}