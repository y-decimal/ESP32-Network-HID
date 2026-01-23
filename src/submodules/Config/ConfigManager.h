#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <interfaces/ISerializable.h>
#include <interfaces/IConfig.h>
#include <submodules/Logger.h>
#include <unordered_map>
#include <typeindex>
#include <vector>

#define ASSERT_ICONFIG static_assert(std::is_base_of<IConfig, T>::value, "T must derive from IConfig interface")
#define ASSERT_NAMESPACE static_assert(has_namespace_static<T>::value, "Config type must define static constexpr const char* NAMESPACE")

static Logger configLog(__FILENAME__);

class ConfigManager : public ISerializable
{
private:
  // Configuration instances
  std::unordered_map<std::string, IConfig *> configMap;

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
  ConfigManager(IStorage &storage) : storage(storage) {};
  ~ConfigManager();

  template <typename T>
  T *createConfig();

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
T *ConfigManager::createConfig()
{
  ASSERT_ICONFIG;
  ASSERT_NAMESPACE;

  std::string key = T::NAMESPACE;
  auto it = configMap.find(key);

  if (it != configMap.end())
  {
    configLog.error("Config %s already exists", T::NAMESPACE);
    return static_cast<T *>(it->second);
  }

  T *cfg = new T();

  cfg->setStorage(&storage);
  configMap[key] = cfg;

  configLog.info("Created new config %s", key);

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
    configLog.error("Could not find config of type %s", key.c_str());
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
    stored = createConfig<T>();
    configLog.info("Config doesn't exist, creating new config");
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

  delete it->second();
  bool erased = configMap.erase(key);

  if (!erased)
    configLog.error("Could not erase config %s", key.c_str());
  else
    configLog.info("Erased config %s", key.c_str());

  return erased;
}

bool ConfigManager::saveConfigs()
{
  bool success = true;
  for (auto it = configMap.begin(); it != configMap.end(); it++)
  {
    if (!it->second->save())
    {
      configLog.error("Config %s could not be saved", it->first.c_str());
      success = false;
    }
  }
  return success;
}

bool ConfigManager::loadConfigs()
{
  bool success = true;
  for (auto it = configMap.begin(); it != configMap.end(); it++)
  {
    if (!it->second->load())
    {
      configLog.error("Config %s could not be loaded", it->first.c_str());
      success = false;
    }
  }
  return success;
}

bool ConfigManager::eraseConfigs()
{
  bool success = true;
  for (auto it = configMap.begin(); it != configMap.end(); it++)
  {
    if (!it->second->erase())
    {
      configLog.error("Config %s could not be erased", it->first.c_str());
      success = false;
    }
  }
  return success;
}

void ConfigManager::clearAllConfigs()
{
  for (auto it = configMap.begin(); it != configMap.end(); it++)
  {
    delete it->second;
  }
  configMap.clear();
}

ConfigManager::~ConfigManager()
{
  clearAllConfigs();
}

#endif