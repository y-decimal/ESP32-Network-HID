#ifndef EVENTTYPES_H
#define EVENTTYPES_H

#include <stdint.h>

struct KeyEvent {
  uint16_t keyIndex;
  bool state;
};

#endif