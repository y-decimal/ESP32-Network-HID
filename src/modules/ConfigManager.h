#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_h

#include <modules/GenericStorage.h>
#include <shared/ConfigTypes.h>

class ConfigManager {
private:
  GenericStorage<uint8_t> hardwareConfig;
};

#endif