#ifndef KEYTYPES_H
#define KEYTYPES_H

#include <stdint.h>

struct KeyEvent {
  uint16_t keyIndex;
  bool state;
};

#endif