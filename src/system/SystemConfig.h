#ifndef SYSTEMCONFIG_H
#define SYSTEMCONFIG_H

// KeyScanner Task Config
static const uint32_t STACK_KEYSCAN = 4096;
static const UBaseType_t PRIORITY_KEYSCAN = 5;
static const BaseType_t CORE_KEYSCAN = 1;
static const TickType_t PERIOD_KEYSCAN_MS = 1;

#endif