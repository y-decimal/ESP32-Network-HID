#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include <cstring>
#include <shared/CommTypes.h>

struct GlobalConfig {

  DeviceRole roles[(size_t)DeviceRole::Count]{};
  MacAddress deviceMac{};

  void setRoles(DeviceRole *roleArray, size_t arrSize);

  void setMac(MacAddress mac);

  void getRoles(DeviceRole *out, size_t size);

  void getMac(uint8_t *out, size_t size);
};

#endif