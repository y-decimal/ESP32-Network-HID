#include "ConfigManager.h"

// Define static members
std::unordered_map<std::string, std::function<IConfig *()>> ConfigManager::factoryMap;
NullStorage ConfigManager::defaultNullStorage;

IConfig *ConfigManager::createConfigByNamespace(const char *namespaceCstring)
{
    std::string namespaceString = namespaceCstring;
    return createConfigByNamespace(namespaceString);
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

bool ConfigManager::saveConfigs()
{
    configLog.debug("Saving configs");
    bool success = true;
    for (auto it = configMap.begin(); it != configMap.end(); it++)
    {
        if (!it->second->save())
        {
            configLog.error("Config %s could not be saved", it->first.c_str());
            success = false;
        }
        else
            configLog.info("Saved config: %s", it->first.c_str());
    }
    return success;
}

bool ConfigManager::loadConfigs()
{
    configLog.debug("Loading configs");
    bool success = true;
    for (auto it = configMap.begin(); it != configMap.end(); it++)
    {
        if (!it->second->load())
        {
            configLog.error("Config %s could not be loaded", it->first.c_str());
            success = false;
        }
        else
            configLog.info("Loaded config: %s", it->first.c_str());
    }
    return success;
}

bool ConfigManager::eraseConfigs()
{
    configLog.debug("Erasing configs");
    bool success = true;
    for (auto it = configMap.begin(); it != configMap.end(); it++)
    {
        if (!it->second->erase())
        {
            configLog.error("Config %s could not be erased", it->first.c_str());
            success = false;
        }
        else
            configLog.info("Erased config: %s", it->first.c_str());
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
        combinedSize += it->first.size();                // namespace string size
        combinedSize += sizeof(size_t);                  // serialized config size info
        combinedSize += it->second->getSerializedSize(); // serialized config size
    }
    return combinedSize;
}

size_t ConfigManager::packSerialized(uint8_t *output, size_t size) const
{
    size_t ownSize = getSerializedSize();

    configLog.info("Serializing configmanager with size %zu Bytes", ownSize);

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
    configLog.info("Deserializing configmanager with size %zu Bytes", size);
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

        sizeOfData = sizeof(size_t);
        size_t configSize = 0;
        memcpy(&configSize, input + read, sizeOfData);
        read += sizeOfData;

        IConfig *cfg = nullptr;
        auto it = configMap.find(namespaceString);
        if (it == configMap.end())
        {
            configLog.info("Config %s does not exist yet, creating new config", namespaceString.c_str());
            cfg = createConfigByNamespace(namespaceString);
            if (cfg == nullptr)
            {
                configLog.error("Failed to deserialize config with namespace %s, it was never registered and will be skipped",
                                namespaceString.c_str());
                read += configSize;
                continue;
            }
        }
        else
        {
            cfg = it->second;
        }

        sizeOfData = configSize;
        cfg->unpackSerialized(input + read, sizeOfData);
        read += sizeOfData;
    }
    return read;
}