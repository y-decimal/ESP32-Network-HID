#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <interfaces/ISerializable.h>
#include <interfaces/IConfig.h>
#include <interfaces/implementations/NullStorage.h>
#include <submodules/Logger.h>
#include <unordered_map>
#include <typeindex>
#include <vector>

#define ASSERT_ICONFIG static_assert(std::is_base_of<IConfig, T>::value, "T must derive from IConfig interface")
#define ASSERT_NAMESPACE static_assert(has_namespace_static<T>::value, "Config type must define static constexpr const char* NAMESPACE")

static Logger configLog("ConfigManager");

class ConfigManager : public ISerializable
{
private:
  // Configuration instances
  std::unordered_map<std::string, IConfig *> configMap;

  // Configuration factories
  static std::unordered_map<std::string, std::function<IConfig *()>> factoryMap;

  static NullStorage defaultNullStorage;
  IStorage &storage;

  void clearAllConfigs();

  template <typename, typename = void>
  struct has_namespace_static : std::false_type
  {
  };

  template <typename T>
  struct has_namespace_static<T, std::void_t<decltype(T::NAMESPACE)>>
      : std::true_type
  {
  };

public:
  ConfigManager(IStorage &storage = defaultNullStorage) : storage(storage)
  {
    if (&this->storage == &defaultNullStorage)
    {
      configLog.debug("ConfigManager initialized with NullStorage: changes will not persist. This might be intentional for local copies");
    }
  };

  ~ConfigManager();

  /**
   * @brief Registers the config so ConfigManager knows it's namespace and how to create it
   */
  template <typename T>
  static void registerConfig();

  template <typename T>
  T *createConfig();

  IConfig *createConfigByNamespace(const char *nameSpaceCstring);

  IConfig *createConfigByNamespace(std::string namespaceString);

  /**
   * @brief Retrieve the configuration of type T.
   * @return The configuration object of type T.
   */
  template <typename T>
  T *getConfig() const;

  /**
   * @brief Set the configuration of type T.
   * @param cfg The configuration object to set.
   */
  template <typename T>
  void setConfig(const T &cfg);

  /**
   * @brief Set the serialized configuration of type T.
   * @param serializedCfg The serialized configuration data.
   */
  template <typename T>
  void setSerializedConfig(const uint8_t *buffer, size_t bufferSize);

  /**
   * @brief Deletes the config of type T
   * @return Returns true if deleted, false if config T does not exist
   */
  template <typename T>
  bool deleteConfig();

  /**
   * @brief Save all configurations to storage.
   * @return True if save was successful, false otherwise.
   */
  bool saveConfigs();

  /**
   * @brief Load all configurations from storage.
   * @return True if load was successful, false otherwise.
   */
  bool loadConfigs();

  /**
   * @brief Erases ALL configurations from PERSISTENT storage.
   * @return True if deletion was successful, false otherwise.
   */
  bool eraseConfigs();

  // Serializable interface implementation
  size_t packSerialized(uint8_t *output, size_t size) const override;
  size_t unpackSerialized(const uint8_t *input, size_t size) override;
  size_t getSerializedSize() const override;
};

// Template implementations

template <typename T>
void ConfigManager::registerConfig()
{
  factoryMap[T::NAMESPACE] = []()
  { return new T(); };
}

template <typename T>
T *ConfigManager::createConfig()
{
  ASSERT_ICONFIG;
  ASSERT_NAMESPACE;

  std::string key = T::NAMESPACE;
  auto it = configMap.find(key);

  if (it != configMap.end())
  {
    configLog.error("Config %s already exists", key.c_str());
    return static_cast<T *>(it->second);
  }

  auto factory = factoryMap.find(T::NAMESPACE);
  if (factory == factoryMap.end())
  {
    configLog.error("Config was not registered. Register config with configManager.registerConfig<CONFIGTYPE>()");
    return nullptr;
  }

  T *cfg = static_cast<T *>(factory->second());
  if (cfg == nullptr)
  {
    configLog.error("Factory for config %s returned nullptr, failed to create config", key.c_str());
    return nullptr;
  }

  cfg->setStorage(&storage);
  configMap[key] = cfg;

  configLog.info("Created new config %s", key.c_str());

  return cfg;
}

template <typename T>
T *ConfigManager::getConfig() const
{
  ASSERT_ICONFIG;
  ASSERT_NAMESPACE;

  std::string key = T::NAMESPACE;
  auto it = configMap.find(key);

  if (it == configMap.end())
  {
    configLog.warn("Could not find config of type %s", key.c_str());
    return nullptr;
  }

  configLog.debug("Returned config of type %s", key.c_str());
  return static_cast<T *>(it->second);
}

template <typename T>
void ConfigManager::setConfig(const T &cfg)
{
  ASSERT_ICONFIG;

  std::vector<uint8_t> buffer(cfg.getSerializedSize());
  cfg.packSerialized(buffer.data(), buffer.size());
  setSerializedConfig<T>(buffer.data(), buffer.size());
}

template <typename T>
void ConfigManager::setSerializedConfig(const uint8_t *buffer, size_t bufferSize)
{
  ASSERT_ICONFIG;

  T *stored = getConfig<T>();
  if (!stored)
  {
    configLog.info("Config doesn't exist, creating new config");
    stored = createConfig<T>();
  }

  if (stored == nullptr)
  {
    configLog.error("Stored Config is nullptr, aborting");
    return;
  }

  stored->unpackSerialized(buffer, bufferSize);
  configLog.info("Set serialized config");
}

template <typename T>
bool ConfigManager::deleteConfig()
{
  ASSERT_ICONFIG;
  ASSERT_NAMESPACE;

  std::string key = T::NAMESPACE;
  auto it = configMap.find(key);

  if (it == configMap.end())
  {
    configLog.error("Cannot delete config of type %s, it does not exist", key.c_str());
    return false;
  }

  delete it->second;
  bool erased = configMap.erase(key);

  if (!erased)
    configLog.error("Could not erase config %s", key.c_str());
  else
    configLog.info("Erased config %s", key.c_str());

  return erased;
}

#endif