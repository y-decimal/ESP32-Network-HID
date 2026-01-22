#include <submodules/Config/ConfigManager.h>

bool ConfigManager::saveConfig()
{
  bool globalSaved = globalCfg.save();
  bool keySaved = keyScannerCfg.save();

  if (!globalSaved)
    configLog.error("Failed to save GlobalConfig");
  if (!keySaved)
    configLog.error("Failed to save KeyScannerConfig");

  configLog.info("Configuration save status: GlobalConfig=%s, KeyScannerConfig=%s",
                 globalSaved ? "success" : "failure",
                 keySaved ? "success" : "failure");

  return globalSaved && keySaved;
}

bool ConfigManager::loadConfig()
{
  bool globalLoaded = false;
  bool keyLoaded = false;

  if (!globalCfg.isDirty())
    globalLoaded = globalCfg.load();
  else
  {
    globalLoaded = false;
    configLog.warn("GlobalConfig dirty, skipping load");
  }

  if (!keyScannerCfg.isDirty())
    keyLoaded = keyScannerCfg.load();
  else
  {
    keyLoaded = false;
    configLog.warn("KeyScannerConfig dirty, skipping load");
  }

  if (!globalLoaded)
    configLog.error("Failed to load GlobalConfig");
  if (!keyLoaded)
    configLog.error("Failed to load KeyScannerConfig");

  configLog.info("Configuration load status: GlobalConfig=%s, KeyScannerConfig=%s",
                 globalLoaded ? "success" : "failure",
                 keyLoaded ? "success" : "failure");

  return globalLoaded && keyLoaded;
}

bool ConfigManager::clearAllConfigs()
{
  bool globalCleared = globalCfg.clearAll();
  bool keyCleared = keyScannerCfg.clearAll();
  if (!globalCleared)
    configLog.error("Failed to clear GlobalConfig");
  if (!keyCleared)
    configLog.error("Failed to clear KeyScannerConfig");
  return globalCleared && keyCleared;
}

size_t ConfigManager::packSerialized(uint8_t *output, size_t size) const
{
  size_t requiredSize = getSerializedSize();
  if (size < requiredSize)
  {
    configLog.error("Insufficient buffer size for serialization: required %zu, provided %zu", requiredSize, size);
    return 0;
  }

  size_t offset = 0;
  uint8_t buffer[requiredSize] = {};

  // Serialize GlobalConfig
  GlobalConfig::SerializedConfig globalSerialized = globalCfg.get();
  memcpy(output + offset, &globalSerialized.size, sizeof(globalSerialized.size));
  offset += sizeof(globalSerialized.size);
  memcpy(output + offset, globalSerialized.data, globalSerialized.size);
  offset += globalSerialized.size;

  // Serialize KeyScannerConfig
  KeyScannerConfig::SerializedConfig keySerialized = keyScannerCfg.get();
  memcpy(output + offset, &keySerialized.size, sizeof(keySerialized.size));
  offset += sizeof(keySerialized.size);
  memcpy(output + offset, keySerialized.data, keySerialized.size);
  offset += keySerialized.size;

  return offset;
}

size_t ConfigManager::unpackSerialized(const uint8_t *input, size_t size)
{
  size_t offset = 0;

  // Deserialize GlobalConfig
  if (offset + sizeof(size_t) > size)
  {
    configLog.error("Insufficient data for GlobalConfig size");
    return 0;
  }
  
  size_t globalSize = 0;
  memcpy(&globalSize, input + offset, sizeof(globalSize));
  offset += sizeof(globalSize);

  if (offset + globalSize > size)
  {
    configLog.error("Insufficient data for GlobalConfig data");
    return 0;
  }

  GlobalConfig::SerializedConfig globalSerialized;
  globalSerialized.size = globalSize;
  memcpy(globalSerialized.data, input + offset, globalSize);
  offset += globalSize;
  globalCfg.set(globalSerialized);

  // Deserialize KeyScannerConfig
  if (offset + sizeof(size_t) > size)
  {
    configLog.error("Insufficient data for KeyScannerConfig size");
    return 0;
  }

  size_t keySize = 0;
  memcpy(&keySize, input + offset, sizeof(keySize));
  offset += sizeof(keySize);

  if (offset + keySize > size)
  {
    configLog.error("Insufficient data for KeyScannerConfig data");
    return 0;
  }

  KeyScannerConfig::SerializedConfig keySerialized;
  keySerialized.size = keySize;
  memcpy(keySerialized.data, input + offset, keySize);
  offset += keySize;
  keyScannerCfg.set(keySerialized);

  return offset;
}

size_t ConfigManager::getSerializedSize() const
{
  // Calculate actual size: 2 size_t fields + actual data from each config
  GlobalConfig::SerializedConfig globalSerialized = globalCfg.get();
  KeyScannerConfig::SerializedConfig keySerialized = keyScannerCfg.get();
  
  return sizeof(size_t) + globalSerialized.size + 
         sizeof(size_t) + keySerialized.size;
}