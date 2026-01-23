#include <submodules/Config/GlobalConfig.h>
#include <submodules/Logger.h>

static Logger log(GlobalConfig::NAMESPACE);

void GlobalConfig::setDeviceModules(DeviceModule *moduleArray, size_t arrSize)
{

  // Prevent overflow
  if (arrSize > (size_t)DeviceModule::Count)
    return;

  // Copy roles into internal array
  memcpy(modules, moduleArray, arrSize);
}

void GlobalConfig::setDeviceMode(DeviceMode mode)
{
  this->mode = mode;
}

void GlobalConfig::setMac(MacAddress mac)
{
  // Copy MAC address into internal variable
  memcpy(deviceMac, mac, 6);
}

void GlobalConfig::getDeviceModules(DeviceModule *out, size_t size)
{
  // Prevent overflow
  if (size < sizeof(modules))
    return;

  // Copy roles into output array
  memcpy(out, modules, sizeof(modules));
}

GlobalConfig::DeviceMode GlobalConfig::getDeviceMode()
{
  return mode;
}

void GlobalConfig::getMac(uint8_t *out, size_t size)
{
  // Prevent overflow
  if (size < sizeof(deviceMac))
    return;

  // Copy MAC address into output array
  memcpy(out, deviceMac, sizeof(deviceMac));
}

// Implementation of IConfig interface methods
void GlobalConfig::setStorage(IStorage *storage)
{
  this->storage = storage;
}

bool GlobalConfig::save()
{
  if (storage == nullptr)
  {
    log.error("No storage backend set, cannot save config");
    return false;
  }

  size_t ownSize = getSerializedSize();
  uint8_t *buffer = (uint8_t *)malloc(ownSize);
  packSerialized(buffer, ownSize);
  bool success = storage->save(NAMESPACE, buffer, ownSize);
  free(buffer);

  success ? log.info("Configuration saved") : log.error("Saving configuration failed");

  return success;
}

bool GlobalConfig::load()
{
  if (storage == nullptr)
  {
    log.error("No storage backend set, cannot load config");
    return false;
  }

  size_t ownSize = storage->getSize(NAMESPACE);
  if (ownSize == 0)
  {
    log.error("No config data stored");
    return false;
  }

  uint8_t *buffer = (uint8_t *)malloc(ownSize);
  bool success = storage->load(NAMESPACE, buffer, ownSize);
  if (!success)
  {
    log.error("Loading config data failed");
    return false;
  }

  size_t packedSize = unpackSerialized(buffer, ownSize);

  if (packedSize != ownSize)
  {
    log.warn("Packed size %d and loaded size %d don't match!", packedSize, ownSize);
  }

  return success;
}

bool GlobalConfig::erase()
{
  return storage->remove(NAMESPACE);
}

// Implementation of Serializable interface methods
size_t GlobalConfig::packSerialized(uint8_t *output, size_t size) const
{

  // Check if provided buffer is large enough
  size_t ownSize = getSerializedSize();
  if (size < ownSize)
    return 0;

  // Helper variables for serialization
  size_t totalWrite = 0;
  size_t objSize = 0;

  // Serialize total config size
  objSize = sizeof(size_t);
  memcpy(output + totalWrite, &ownSize, objSize);
  totalWrite += objSize;

  // Serialize module size
  objSize = sizeof(size_t);
  size_t moduleSize = sizeof(modules);
  memcpy(output + totalWrite, &moduleSize, objSize);
  totalWrite += objSize;

  // Serialize module
  objSize = moduleSize;
  memcpy(output, modules, objSize);
  totalWrite += objSize;

  // Serialize mode
  objSize = sizeof(mode);
  memcpy(output + totalWrite, &mode, objSize);
  totalWrite += objSize;

  // Serialize MAC address
  objSize = sizeof(deviceMac);
  memcpy(output + totalWrite, deviceMac, objSize);
  totalWrite += objSize;

  return totalWrite;
}

size_t GlobalConfig::unpackSerialized(const uint8_t *input, size_t size)
{

  // Helper variables for deserialization
  size_t totalRead = 0;
  size_t objSize = 0;

  size_t ownSize = 0;
  objSize = sizeof(size_t);
  memcpy(&ownSize, input + totalRead, objSize);

  // Check if provided data size is valid
  if (size > ownSize)
    return 0;

  // Deserialize module size
  size_t moduleSize = 0;
  objSize = sizeof(size_t);
  memcpy(&moduleSize, input + totalRead, objSize);
  totalRead += objSize;

  // Deserialize modules
  objSize = moduleSize;
  memcpy(modules, input + totalRead, objSize);
  totalRead += objSize;

  // Deserialize mode
  objSize = sizeof(mode);
  memcpy(&mode, input + totalRead, objSize);
  totalRead += objSize;

  // Deserialize MAC address
  objSize = sizeof(deviceMac);
  memcpy(deviceMac, input + totalRead, objSize);
  totalRead += objSize;

  return totalRead;
}

size_t GlobalConfig::getSerializedSize() const
{
  // Return the total size needed for serialization
  // Size metadata of total size + size metadata of module size + module data + mode data + mac data
  return sizeof(size_t) + sizeof(size_t) + sizeof(modules) + sizeof(mode) + sizeof(deviceMac);
}