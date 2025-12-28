#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_h

#include <shared/ConfigTypes.h>
#include <submodules/GenericStorage.h>

class ConfigManager {
private:
  GlobalConfig globalCfg;
  KeyScannerConfig keyScannerCfg;

public:
  GlobalConfig getGlobalConfig() const;
  KeyScannerConfig getKeyConfig() const;

  void setGlobalConfig(GlobalConfig cfg);
  void setKeyConfig(KeyScannerConfig cfg);
};

#endif