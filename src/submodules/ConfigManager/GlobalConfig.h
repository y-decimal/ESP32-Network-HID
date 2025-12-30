#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include <cstring>
#include <interfaces/ISerializableStructs.h>
#include <shared/CommTypes.h>

struct GlobalConfig : public Serializable {

  DeviceRole roles[(size_t)DeviceRole::Count]{};
  MacAddress deviceMac{};

  void setRoles(DeviceRole *roleArray, size_t arrSize);

  void setMac(MacAddress mac);

  void getRoles(DeviceRole *out, size_t size);

  void getMac(uint8_t *out, size_t size);

  size_t packSerialized(uint8_t *output, size_t size) const override;
  size_t unpackSerialized(const uint8_t *input, size_t size) override;
  size_t getSerializedSize() const override;
};

#endif