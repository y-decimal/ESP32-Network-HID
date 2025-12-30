#include <submodules/ConfigManager/GlobalConfig.h>

void GlobalConfig::setRoles(DeviceRole *roleArray, size_t arrSize) {
  if (arrSize > (size_t)DeviceRole::Count)
    return;
  memcpy(roles, roleArray, arrSize);
}

void GlobalConfig::setMac(MacAddress mac) { memcpy(deviceMac, mac, 6); }

void GlobalConfig::getRoles(DeviceRole *out, size_t size) {
  if (size < sizeof(roles))
    return;
  memcpy(out, roles, sizeof(roles));
}

void GlobalConfig::getMac(uint8_t *out, size_t size) {
  if (size < sizeof(deviceMac))
    return;
  memcpy(out, deviceMac, sizeof(deviceMac));
}

size_t GlobalConfig::packSerialized(uint8_t *output, size_t size) const {
  size_t ownSize = getSerializedSize();
  if (size < ownSize)
    return 0;
  uint8_t buffer[ownSize] = {};

  size_t index = 0;
  size_t totalWrite = 0;
  size_t objSize = 0;

  objSize = sizeof(roles);
  memcpy(buffer, roles, objSize);
  index += objSize;
  totalWrite += objSize;

  objSize = sizeof(deviceMac);
  memcpy(buffer + index, deviceMac, objSize);
  totalWrite += objSize;

  memcpy(output, buffer, totalWrite);

  return totalWrite;
}

size_t GlobalConfig::unpackSerialized(const uint8_t *input, size_t size) {
  size_t ownSize = getSerializedSize();
  if (size > ownSize)
    return 0;

  size_t index = 0;
  size_t totalWrite = 0;
  size_t objSize = 0;

  objSize = sizeof(roles);
  memcpy(roles, input, objSize);
  index += objSize;
  totalWrite += objSize;

  objSize = sizeof(deviceMac);
  memcpy(deviceMac, input + index, objSize);
  totalWrite += objSize;

  return totalWrite;
}

size_t GlobalConfig::getSerializedSize() const {
  return sizeof(roles) + sizeof(deviceMac);
}