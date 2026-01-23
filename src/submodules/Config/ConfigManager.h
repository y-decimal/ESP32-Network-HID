#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <interfaces/ISerializableStructs.h>
#include <interfaces/IConfig.h>
#include <submodules/Logger.h>
#include <unordered_map>
#include <typeindex>
#include <vector>

#define ASSERT_ICONFIG static_assert(std::is_base_of<IConfig, T>::value, "T must derive from IConfig interface")

static Logger configLog(__FILENAME__);

class ConfigManager : public ISerializable
{
private:
  // Configuration instances
  std::unordered_map<std::type_index, IConfig *> configMap;

public:
  ConfigManager();
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
   * @brief Clear all configurations from storage.
   * @return True if clear was successful, false otherwise.
   */
  void clearAllConfigs();

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

  auto key = std::type_index(typeid(T));
  auto it = configMap.find(key);

  if (it != configMap.end())
  {
    configLog.error("Config %s already exists", typeid(T).name());
    return static_cast<T *>(it->second);
  }

  T *cfg = new T();
  configMap[key] = cfg;

  configLog.info("Created new config %s", typeid(T).name());

  return cfg;
}

template <typename T>
T *ConfigManager::getConfig() const
{
  ASSERT_ICONFIG;

  auto it = configMap.find(std::type_index(typeid(T)));

  if (it == configMap.end())
  {
    configLog.error("Could not find config of type %s", typeid(T).name());
    return nullptr;
  }

  configLog.debug("Returned config of type %s", typeid(T).name());
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

bool ConfigManager::saveConfigs()
{
  bool success = true;
  for (auto it = configMap.begin(); it != configMap.end(); it++)
  {
    if (!it->second->save())
    {
      configLog.error("Config %s could not be saved", typeid(*it->second).name());
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
      configLog.error("Config %s could not be loaded", typeid(*it->second).name());
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