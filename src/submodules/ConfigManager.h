#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <shared/ConfigTypes.h>
#include <submodules/GenericStorage.h>

class ConfigManager {
private:
  GenericStorage<GlobalConfig> globalCfg =
      GenericStorage<GlobalConfig>("globalCfg");
  GenericStorage<KeyScannerConfig> keyScannerCfg =
      GenericStorage<KeyScannerConfig>("keyScannerCfg");

public:
  GlobalConfig getGlobalConfig() const;
  KeyScannerConfig getKeyConfig() const;

  void setGlobalConfig(GlobalConfig cfg);
  void setKeyConfig(KeyScannerConfig cfg);

  bool saveConfig();
  bool loadConfig();
};

#endif