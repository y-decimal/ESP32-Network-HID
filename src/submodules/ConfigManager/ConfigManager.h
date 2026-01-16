#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <interfaces/IStorage.h>
#include <submodules/ConfigManager/GlobalConfig.h>
#include <submodules/ConfigManager/KeyScannerConfig.h>
#include <submodules/Storage/TGenericStorage.h>

// Namespaces for configuration manager storage keys
#define CONFIG_MANAGER_NAMESPACE "ConfigManager"
#define GLOBAL_CONFIG_KEY "globalCfg"
#define KEYSCANNER_CONFIG_KEY "keyScannerCfg"

/**
 * @brief Configuration Manager for handling different configuration types.
 *
 * The ConfigManager class provides methods to get, set, save, and load
 * configurations for various components such as GlobalConfig and
 * KeyScannerConfig. It utilizes a thread-safe generic storage mechanism to
 * ensure safe access to configuration data.
 */
class ConfigManager {
private:
  // Reference to the storage interface
  IStorage &storage;

  // Thread-safe storage for different configuration types
  ThreadSafeGenericStorage<GlobalConfig::SerializedConfig> globalCfg{
      storage, CONFIG_MANAGER_NAMESPACE "/" GLOBAL_CONFIG_KEY};
  ThreadSafeGenericStorage<KeyScannerConfig::SerializedConfig> keyScannerCfg{
      storage, CONFIG_MANAGER_NAMESPACE "/" KEYSCANNER_CONFIG_KEY};

public:
  /**
   * @brief Constructor for ConfigManager.
   * @param storage Reference to an IStorage implementation for data operations.
   */
  ConfigManager(IStorage &storage) : storage(storage) {}

  /**
   * @brief Retrieve the configuration of type T.
   * @return The configuration object of type T.
   */
  template <typename T> T getConfig() const;

  /**
   * @brief Set the configuration of type T.
   * @param cfg The configuration object to set.
   */
  template <typename T> void setConfig(const T &cfg);

  /**
   * @brief Save all configurations to storage.
   * @return True if save was successful, false otherwise.
   */
  bool saveConfig();

  /**
   * @brief Load all configurations from storage.
   * @return True if load was successful, false otherwise.
   */
  bool loadConfig();
};

// Template specializations

/**
 * @brief Retrieve the GlobalConfig configuration.
 * @return The GlobalConfig object.
 */
template <> inline GlobalConfig ConfigManager::getConfig<GlobalConfig>() const {
  GlobalConfig::SerializedConfig serialized = globalCfg.get();
  GlobalConfig config; // Create with defaults
  // Only unpack if we have valid data (size > 0)
  if (serialized.size > 0) {
    config.unpackSerialized(serialized.data, serialized.size);
  }
  return config;
}

/**
 * @brief Retrieve the KeyScannerConfig configuration.
 * @return The KeyScannerConfig object.
 */
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

/**
 * @brief Set the GlobalConfig configuration.
 * @param cfg The GlobalConfig object to set.
 */
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

/**
 * @brief Set the KeyScannerConfig configuration.
 * @param cfg The KeyScannerConfig object to set.
 */
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