#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_h

#include <submodules/GenericStorage.h>
#include <shared/ConfigTypes.h>

class ConfigManager {
private:
  GlobalConfig globalCfg;
  KeyScannerConfig keyScannerCfg;

public:
  GlobalConfig getGlobalConfig() const;
  KeyScannerConfig getKeyConfig() const;
};

#endif