#ifndef TASKPARAMETERS_H
#define TASKPARAMETERS_H

#include <shared/DataTypes.h>
#include <submodules/ConfigManager.h>

struct KeyScannerParameters {
  KeyScannerConfig *config;
  KeyScannerState *state;
};

#endif