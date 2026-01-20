#include <submodules/ConfigManager/GlobalConfig.h>

void GlobalConfig::setRoles(DeviceRole *roleArray, size_t arrSize) {

  // Prevent overflow
  if (arrSize > (size_t)DeviceRole::Count)
    return;

  // Copy roles into internal array
  memcpy(roles, roleArray, arrSize);
}

void GlobalConfig::setMac(MacAddress mac) {
  // Copy MAC address into internal variable
  memcpy(deviceMac, mac, 6);
}

void GlobalConfig::getRoles(DeviceRole *out, size_t size) {
  // Prevent overflow
  if (size < sizeof(roles))
    return;

  // Copy roles into output array
  memcpy(out, roles, sizeof(roles));
}

void GlobalConfig::getMac(uint8_t *out, size_t size) {
  // Prevent overflow
  if (size < sizeof(deviceMac))
    return;

  // Copy MAC address into output array
  memcpy(out, deviceMac, sizeof(deviceMac));
}

// Implementation of Serializable interface methods
size_t GlobalConfig::packSerialized(uint8_t *output, size_t size) const {

  // Check if provided buffer is large enough
  size_t ownSize = getSerializedSize();
  if (size < ownSize)
    return 0;

  // Temporary buffer to hold serialized data
  uint8_t buffer[ownSize] = {};

  // Helper variables for serialization
  size_t index = 0;
  size_t totalWrite = 0;
  size_t objSize = 0;

  // Serialize roles
  objSize = sizeof(roles);
  memcpy(buffer, roles, objSize);
  index += objSize;
  totalWrite += objSize;

  // Serialize MAC address
  objSize = sizeof(deviceMac);
  memcpy(buffer + index, deviceMac, objSize);
  totalWrite += objSize;

  // Copy serialized data to output buffer
  memcpy(output, buffer, totalWrite);

  return totalWrite;
}

size_t GlobalConfig::unpackSerialized(const uint8_t *input, size_t size) {

  // Check if provided data size is valid
  size_t ownSize = getSerializedSize();
  if (size > ownSize)
    return 0;

  // Helper variables for deserialization
  size_t index = 0;
  size_t totalWrite = 0;
  size_t objSize = 0;

  // Deserialize roles
  objSize = sizeof(roles);
  memcpy(roles, input, objSize);
  index += objSize;
  totalWrite += objSize;

  // Deserialize MAC address
  objSize = sizeof(deviceMac);
  memcpy(deviceMac, input + index, objSize);
  totalWrite += objSize;

  return totalWrite;
}

size_t GlobalConfig::getSerializedSize() const {
  // Return the total size needed for serialization
  return sizeof(roles) + sizeof(deviceMac);
}