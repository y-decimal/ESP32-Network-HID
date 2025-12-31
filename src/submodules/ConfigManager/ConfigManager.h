#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <submodules/ConfigManager/GlobalConfig.h>
#include <submodules/ConfigManager/KeyScannerConfig.h>
#include <submodules/Storage/PreferencesStorage.h>
#include <submodules/Storage/TGenericStorage.h>

class ConfigManager {
private:
  PreferencesStorage storage{"ConfigManager"};

  ThreadSafeGenericStorage<GlobalConfig::SerializedConfig> globalCfg{
      storage, "globalCfg"};
  ThreadSafeGenericStorage<KeyScannerConfig::SerializedConfig> keyScannerCfg{
      storage, "keyScannerCfg"};

public:
  template <typename T> T getConfig() const;

  template <typename T> void setConfig(const T &cfg);

  bool saveConfig();
  bool loadConfig();
};

// Template specializations
template <> inline GlobalConfig ConfigManager::getConfig<GlobalConfig>() const {
  GlobalConfig::SerializedConfig serialized = globalCfg.get();
  GlobalConfig config; // Create with defaults
  // Only unpack if we have valid data (size > 0)
  if (serialized.size > 0) {
    config.unpackSerialized(serialized.data, serialized.size);
  }
  return config;
}

template <>
inline KeyScannerConfig ConfigManager::getConfig<KeyScannerConfig>() const {
  KeyScannerConfig::SerializedConfig serialized = keyScannerCfg.get();
  KeyScannerConfig config; // Create with defaults
  // Only unpack if we have valid data (size > 0)
  if (serialized.size > 0) {
    config.unpackSerialized(serialized.data, serialized.size);
  }
  return config;
}

template <>
inline void ConfigManager::setConfig<GlobalConfig>(const GlobalConfig &cfg) {
  GlobalConfig::SerializedConfig serialized;
  serialized.size =
      cfg.packSerialized(serialized.data, sizeof(serialized.data));
  if (serialized.size == 0) {
    printf("ERROR: Failed to serialize GlobalConfig\n");
    return;
  }
  globalCfg.set(serialized);
}

template <>
inline void
ConfigManager::setConfig<KeyScannerConfig>(const KeyScannerConfig &cfg) {
  KeyScannerConfig::SerializedConfig serialized;
  serialized.size =
      cfg.packSerialized(serialized.data, sizeof(serialized.data));
  if (serialized.size == 0) {
    printf("ERROR: Failed to serialize KeyScannerConfig\n");
    return;
  }
  keyScannerCfg.set(serialized);
}

#endif