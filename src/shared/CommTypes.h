#ifndef COMMTYPES_H
#define COMMTYPES_H

#include <stdint.h>

typedef uint8_t MacAddress[6];

enum class NodeID : uint8_t
{
  // Fixed NodeIDs, number of elements limits maximum concurrent connections
  // 10 should be enough, but can easily be increased if necessary
  NodeMaster,
  Node1,
  Node2,
  Node3,
  Node4,
  Node5,
  Node6,
  Node7,
  Node8,
  Node9,
  Count
};

enum class DeviceRole : uint8_t
{
  Master,
  Keyboard,
  Count
};

enum class PacketType : uint8_t
{
  KeyEvent,
  KeyBitmap,
  Config,
  PairingRequest,
  PairingConfirmation,
  Count
};

#endif