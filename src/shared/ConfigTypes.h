#ifndef CONFIGTYPES_H
#define CONFIGTYPES_H

#include <cstring>
#include <interfaces/ISerializableStructs.h>
#include <shared/CommTypes.h>
#include <stdint.h>
#include <vector>



static constexpr uint8_t MAX_PIN_COUNT = 20;

struct GlobalConfig {
  DeviceRole roles[(size_t)DeviceRole::Count]{};
  MacAddress deviceMac{};

  void setRoles(DeviceRole *roleArray, size_t arrSize) {
    if (arrSize > (size_t)DeviceRole::Count)
      return;
    memcpy(roles, roleArray, arrSize);
  }

  void setMac(MacAddress mac) { memcpy(deviceMac, mac, 6); }

  void getRoles(DeviceRole *out, size_t size) {
    if (size < sizeof(roles))
      return;
    memcpy(out, roles, sizeof(roles));
  }

  void getMac(uint8_t *out, size_t size) {
    if (size < sizeof(deviceMac))
      return;
    memcpy(out, deviceMac, sizeof(deviceMac));
  }
};



#endif