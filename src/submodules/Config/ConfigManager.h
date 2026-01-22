#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <interfaces/IStorage.h>
#include <interfaces/ISerializableStructs.h>
#include <submodules/Config/GlobalConfig.h>
#include <submodules/Config/KeyScannerConfig.h>
#include <submodules/Storage/TGenericStorage.h>
#include <submodules/Logger.h>

// Storage keys for configuration manager (max 15 chars for ESP32 Preferences)
#define CONFIG_MANAGER_NAMESPACE "CfgMgr"
#define GLOBAL_CONFIG_KEY "global"
#define KEYSCANNER_CONFIG_KEY "keyScan"

static Logger configLog(CONFIG_MANAGER_NAMESPACE);

/**
 * @brief Configuration Manager for handling different configuration types.
 *
 * The ConfigManager class provides methods to get, set, save, and load
 * configurations for various components such as GlobalConfig and
 * KeyScannerConfig. It utilizes a thread-safe generic storage mechanism to
 * ensure safe access to configuration data.
 */
class ConfigManager : public Serializable
{
private:
  // Reference to the storage interface

  // Thread-safe storage for different configuration types
  ThreadSafeGenericStorage<GlobalConfig::SerializedConfig> globalCfg;
  ThreadSafeGenericStorage<KeyScannerConfig::SerializedConfig> keyScannerCfg;

public:

  struct SerializedConfig
  {
    uint8_t data[sizeof(GlobalConfig::SerializedConfig) +
                 sizeof(KeyScannerConfig::SerializedConfig)]{0};
    size_t size = sizeof(data);
  };

  /**
   * @brief Constructor for ConfigManager.
   * @param storage Reference to an IStorage implementation for data operations.
   * If left empty ConfigManager will start in volatile storage mode
   */
  ConfigManager(IStorage *storageBackend = nullptr)
      : globalCfg(CONFIG_MANAGER_NAMESPACE "/" GLOBAL_CONFIG_KEY, storageBackend),
        keyScannerCfg(CONFIG_MANAGER_NAMESPACE "/" KEYSCANNER_CONFIG_KEY, storageBackend) {}

  /**
   * @brief Retrieve the configuration of type T.
   * @return The configuration object of type T.
   */
  template <typename T>
  T getConfig() const;

  /**
   * @brief Set the configuration of type T.
   * @param cfg The configuration object to set.
   */
  template <typename T>
  void setConfig(const T &cfg);

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

  /**
   * @brief Clear all configurations from storage.
   * @return True if clear was successful, false otherwise.
   */
  bool clearAllConfigs();

  // Serializable interface implementation
  size_t packSerialized(uint8_t *output, size_t size) const override;
  size_t unpackSerialized(const uint8_t *input, size_t size) override;
  size_t getSerializedSize() const override;
};

// Template specializations

/**
 * @brief Retrieve the GlobalConfig configuration.
 * @return The GlobalConfig object.
 */
template <>
inline GlobalConfig ConfigManager::getConfig<GlobalConfig>() const
{
  GlobalConfig::SerializedConfig serialized = globalCfg.get();
  GlobalConfig config; // Create with defaults
  // Only unpack if we have valid data (size > 0)
  if (serialized.size > 0)
  {
    config.unpackSerialized(serialized.data, serialized.size);
  }
  else 
  {
    configLog.warn("GlobalConfig is empty, using defaults");
  }
  return config;
}

/**
 * @brief Retrieve the KeyScannerConfig configuration.
 * @return The KeyScannerConfig object.
 */
template <>
inline KeyScannerConfig ConfigManager::getConfig<KeyScannerConfig>() const
{
  KeyScannerConfig::SerializedConfig serialized = keyScannerCfg.get();
  KeyScannerConfig config; // Create with defaults
  // Only unpack if we have valid data (size > 0)
  if (serialized.size > 0)
  {
    config.unpackSerialized(serialized.data, serialized.size);
  }
  else 
  {
    configLog.warn("KeyScannerConfig is empty, using defaults");
  }
  return config;
}

/**
 * @brief Set the GlobalConfig configuration.
 * @param cfg The GlobalConfig object to set.
 */
template <>
inline void ConfigManager::setConfig<GlobalConfig>(const GlobalConfig &cfg)
{
  GlobalConfig::SerializedConfig serialized;
  serialized.size = cfg.packSerialized(serialized.data, sizeof(serialized.data));
  if (serialized.size == 0)
  {
    configLog.error("Failed to serialize GlobalConfig");
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
ConfigManager::setConfig<KeyScannerConfig>(const KeyScannerConfig &cfg)
{
  KeyScannerConfig::SerializedConfig serialized;
  serialized.size = cfg.packSerialized(serialized.data, sizeof(serialized.data));
  if (serialized.size == 0)
  {
    configLog.error("Failed to serialize KeyScannerConfig");
    return;
  }
  keyScannerCfg.set(serialized);
}

#endif