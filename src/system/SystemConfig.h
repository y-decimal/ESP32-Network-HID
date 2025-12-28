#ifndef SYSTEMCONFIG_H
#define SYSTEMCONFIG_H

// Priority Event Task Config
static const uint32_t STACK_PRIORITYEVENT = 4096;
static const UBaseType_t PRIORITY_PRIORITYEVENT = 4;
static const BaseType_t CORE_PRIORITYEVENT = 1;

// KeyScanner Task Config
static const uint32_t STACK_KEYSCAN = 4096;
static const UBaseType_t PRIORITY_KEYSCAN = 5;
static const BaseType_t CORE_KEYSCAN = 1;
static const TickType_t PERIOD_KEYSCAN_MS = 1;

#endif