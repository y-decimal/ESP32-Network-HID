#ifndef COMMTYPES_H
#define COMMTYPES_H

#include <stdint.h>

typedef uint8_t MacAddress[6];

enum class DeviceRole : uint8_t
{
  Master,
  Keyboard,
  Count
};

#endif