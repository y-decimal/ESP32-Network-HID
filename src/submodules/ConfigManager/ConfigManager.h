#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <submodules/ConfigManager/GlobalConfig.h>
#include <submodules/ConfigManager/KeyScannerConfig.h>
#include <submodules/TGenericStorage.h>

class ConfigManager {
private:
  ThreadSafeGenericStorage<GlobalConfig::SerializedConfig> globalCfg{
      "globalCfg"};
  ThreadSafeGenericStorage<KeyScannerConfig::SerializedConfig> keyScannerCfg{
      "keyScannerCfg"};

public:
  template <typename T> T getConfig() const;

  template <typename T> void setConfig(const T &cfg);

  bool saveConfig();
  bool loadConfig();
};

// Template specializations
template <> inline GlobalConfig ConfigManager::getConfig<GlobalConfig>() const {
  GlobalConfig::SerializedConfig serialized = globalCfg.get();
  GlobalConfig globalCfg;
  globalCfg.unpackSerialized(serialized.data, serialized.size);
  return globalCfg;
}

template <>
inline KeyScannerConfig ConfigManager::getConfig<KeyScannerConfig>() const {
  KeyScannerConfig::SerializedConfig serialized = keyScannerCfg.get();
  KeyScannerConfig keyScannerCfg;
  keyScannerCfg.unpackSerialized(serialized.data, serialized.size);
  return keyScannerCfg;
}

template <>
inline void ConfigManager::setConfig<GlobalConfig>(const GlobalConfig &cfg) {
  GlobalConfig::SerializedConfig serialized;
  serialized.size = cfg.packSerialized(serialized.data, sizeof(serialized.data));
  globalCfg.set(serialized);
}

template <>
inline void
ConfigManager::setConfig<KeyScannerConfig>(const KeyScannerConfig &cfg) {
  KeyScannerConfig::SerializedConfig serialized;
  serialized.size = cfg.packSerialized(serialized.data, sizeof(serialized.data));
  keyScannerCfg.set(serialized);
}

#endif