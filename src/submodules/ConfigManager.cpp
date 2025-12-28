#include <submodules/ConfigManager.h>

bool ConfigManager::saveConfig() {

  bool globalSaved = false;
  bool keySaved = false;
  bool bitmapSaved = false;

  if (globalCfg.isDirty())
    globalSaved = globalCfg.save();
  else
    globalSaved = true;
  if (keyScannerCfg.isDirty())
    keySaved = keyScannerCfg.save();
  else
    keySaved = true;
  if (bitmapCfg.isDirty())
    bitmapSaved = bitmapCfg.save();
  else
    bitmapSaved = true;
  return globalSaved && keySaved && bitmapSaved;
}

bool ConfigManager::loadConfig() {

  bool globalLoaded = false;
  bool keyLoaded = false;
  bool bitmapLoaded = false;

  if (!globalCfg.isDirty())
    globalLoaded = globalCfg.load();
  else
    globalLoaded = true;
  if (!keyScannerCfg.isDirty())
    keyLoaded = keyScannerCfg.load();
  else
    keyLoaded = true;
  if (!bitmapCfg.isDirty())
    bitmapLoaded = bitmapCfg.load();
  else
    bitmapLoaded = true;

  return globalLoaded && keyLoaded && bitmapLoaded;
}