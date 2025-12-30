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