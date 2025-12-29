#ifndef DATATYPES_H
#define DATATYPES_H

#include <cstring>
#include <stdint.h>

struct KeyScannerState {
  uint8_t *bitMap;
  uint8_t bitMapSize;
};

class IBitMapRoutingStrategy {
public:
  virtual void routeBitMap(const uint8_t *data, uint8_t size) = 0;
  virtual ~IBitMapRoutingStrategy() = default;
};

class InternalBitMapRouting : public IBitMapRoutingStrategy {
  static int timesInvokedSincePrint;
  static int timesInvokedTotal;

public:
  void routeBitMap(const uint8_t *data, uint8_t size) override {
    // dummy implementation
    if (!timesInvokedSincePrint)
      timesInvokedSincePrint = 0;

    if (!timesInvokedTotal)
      timesInvokedTotal = 0;

    timesInvokedSincePrint++;
    timesInvokedTotal++;
    if (timesInvokedSincePrint > 8) {
      printf("[InternalBitMapRouting] Routing invoked %d times\n",
             timesInvokedTotal);
    }
  }
};

class NetworkBitMapRouting : public IBitMapRoutingStrategy {
public:
  void routeBitMap(const uint8_t *data, uint8_t size) override {
    // Implement network routing
  }
};

#endif