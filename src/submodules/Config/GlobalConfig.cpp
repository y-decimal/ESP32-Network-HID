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
  return false; // Todo
}

bool GlobalConfig::load()
{
  return false; // Todo
}

bool GlobalConfig::erase()
{
  return false; // Todo
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

  // Check if provided data size is valid
  size_t ownSize = getSerializedSize();
  if (size > ownSize)
    return 0;

  // Helper variables for deserialization
  size_t index = 0;
  size_t totalWrite = 0;
  size_t objSize = 0;

  // Deserialize modules
  objSize = sizeof(modules);
  memcpy(modules, input, objSize);
  index += objSize;
  totalWrite += objSize;

  // Deserialize mode
  objSize = sizeof(mode);
  memcpy(&mode, input + index, objSize);
  index += objSize;
  totalWrite += objSize;

  // Deserialize MAC address
  objSize = sizeof(deviceMac);
  memcpy(deviceMac, input + index, objSize);
  totalWrite += objSize;

  return totalWrite;
}

size_t GlobalConfig::getSerializedSize() const
{
  // Return the total size needed for serialization
  return sizeof(modules) + sizeof(mode) + sizeof(deviceMac);
}