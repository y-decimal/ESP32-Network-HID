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

  // Configuration factories
  std::unordered_map<std::string, std::function<IConfig *()>> factoryMap;

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

  /**
   * @brief Registers the config so ConfigManager knows it's namespace and how to create it
   */
  template <typename T>
  void registerConfig();

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
  factoryMap[T::NAMESPACE] = {
    []() return new T();
};
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
    configLog.error("Config %s already exists", T::NAMESPACE);
    return static_cast<T *>(it->second);
  }

  auto it = factoryMap.find(T::NAMESPACE) if (it == factoryMap.end())
  {
    configLog.error("Config was not registered. Register config with configManager.registerConfig<CONFIGTYPE>()");
    return nullptr;
  }

  T *cfg = it->second();
  if (cfg == nullptr)
  {
    configLog.error("Factory for config %s returned nullptr, failed to create config", T::NAMESPACE);
    return nullptr;
  }

  cfg->setStorage(&storage);
  configMap[key] = cfg;

  configLog.info("Created new config %s", key);

  return cfg;
}

IConfig *ConfigManager::createConfigByNamespace(const char *namespaceCstring)
{
  std::string namespaceString = namespaceCstring;
  createConfigByNamespace(namespaceString);
}

IConfig *ConfigManager::createConfigByNamespace(std::string namespaceString)
{
  auto it = factoryMap.find(namespaceString);
  if (it == factoryMap.end())
  {
    configLog.error("Could not create config with namespace %s, it was never registered",
                    namespaceString.c_str());
    return nullptr;
  }

  IConfig *cfg = it->second();
  if (cfg == nullptr)
    configLog.error("Factory for config with namespace %s returned nullptr", namespaceString.c_str());

  configMap[namespaceString] = cfg;
  cfg->setStorage(&storage);

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

size_t ConfigManager::getSerializedSize() const
{
  size_t combinedSize = 0;
  for (auto it = configMap.begin(); it != configMap.end(); it++)
  {
    combinedSize += sizeof(size_t);                  // namespace string size info
    combinedSize += sizeof(it->first.size());        // namespace string size
    combinedSize += sizeof(size_t);                  // serialized config size info
    combinedSize += it->second->getSerializedSize(); // serialized config size
  }
  return combinedSize;
}

size_t ConfigManager::packSerialized(uint8_t *output, size_t size) const
{
  size_t ownSize = getSerializedSize();

  configLog.info("Serializing configmanager with size %d Bytes", ownSize);

  if (size < ownSize)
  {
    configLog.error("Could not serialize configmanager, output buffer too small");
    return 0;
  }

  size_t written = 0;

  for (auto it = configMap.begin(); it != configMap.end(); it++)
  {
    size_t sizeOfData = it->first.size();                  // Get size of namespace string
    memcpy(output + written, &sizeOfData, sizeof(size_t)); // Write size metadata field
    written += sizeof(size_t);
    memcpy(output + written, it->first.c_str(), sizeOfData); // Write namespace string
    written += sizeOfData;

    sizeOfData = it->second->getSerializedSize();          // Get size of serialized config
    memcpy(output + written, &sizeOfData, sizeof(size_t)); // Write size metadata field
    written += sizeof(size_t);
    it->second->packSerialized(output + written, sizeOfData); // Write serialized config
    written += sizeOfData;
  }

  return written;
}

size_t ConfigManager::unpackSerialized(const uint8_t *input, size_t size)
{
  configLog.info("Deserializing configmanager with size %d Bytes", size);
  size_t read = 0;

  while (read < size)
  {
    size_t sizeOfData = sizeof(size_t);
    size_t stringSize = 0;
    memcpy(&stringSize, input + read, sizeOfData);
    read += sizeOfData;

    sizeOfData = stringSize;
    std::string namespaceString;
    namespaceString.resize(stringSize);
    memcpy(&namespaceString[0], input + read, sizeOfData);
    read += sizeOfData;

    IConfig *cfg = nullptr;
    auto it = configMap.find(namespaceString);
    if (it == configMap.end())
    {
      configLog.info("Config %s does not exist yet, creating new config", namespaceString.c_str());
      cfg = createConfigByNamespace(namespaceString);
      if (cfg == nullptr)
      {
        configLog.error("Failed to deserialize config with namespace %s, it was never registered",
                        namespaceString.c_str());
        return 0;
      }
    }
    else
    {
      cfg = it->second;
    }

    sizeOfData = sizeof(size_t);
    size_t configSize = 0;
    memcpy(&configSize, input + read, sizeOfData);
    read += sizeOfData;

    sizeOfData = configSize;
    cfg->unpackSerialized(input + read, sizeOfData);
    read += sizeOfData;
  }
  return read;
}

#endif