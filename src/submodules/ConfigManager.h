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
  GenericStorage<BitMapSenderConfig> bitmapCfg =
      GenericStorage<BitMapSenderConfig>("bitmapCfg");

public:
  template <typename T> T getConfig() const;

  template <typename T> void setConfig(const T &cfg);

  bool saveConfig();
  bool loadConfig();
};

// Template specializations
template <> inline GlobalConfig ConfigManager::getConfig<GlobalConfig>() const {
  return globalCfg.get();
}

template <>
inline KeyScannerConfig ConfigManager::getConfig<KeyScannerConfig>() const {
  return keyScannerCfg.get();
}

template <>
inline BitMapSenderConfig ConfigManager::getConfig<BitMapSenderConfig>() const {
  return bitmapCfg.get();
}

template <>
inline void ConfigManager::setConfig<GlobalConfig>(const GlobalConfig &cfg) {
  globalCfg.set(cfg);
}

template <>
inline void
ConfigManager::setConfig<KeyScannerConfig>(const KeyScannerConfig &cfg) {
  keyScannerCfg.set(cfg);
}

template <>
inline void
ConfigManager::setConfig<BitMapSenderConfig>(const BitMapSenderConfig &cfg) {
  bitmapCfg.set(cfg);
}

#endif