#ifndef SYSTEMCONFIG_H
#define SYSTEMCONFIG_H

#include <FreeRTOS.h>
#include <stdint.h>
#include <task.h>

// Global constants
static const uint8_t BITMAPSIZE =
    6; // 6 Bytes for 127 bits, should be enough for every possible key

// Priority Event Task Config
static const uint32_t STACK_PRIORITYEVENT = 4096;
static const UBaseType_t PRIORITY_PRIORITYEVENT = 4;
static const BaseType_t CORE_PRIORITYEVENT = 1;

// KeyScanner Task Config
static const uint32_t STACK_KEYSCAN = 4096;
static const UBaseType_t PRIORITY_KEYSCAN = 5;
static const BaseType_t CORE_KEYSCAN = 1;

// BitMapSender Task Config
static const uint32_t STACK_BITMAP = 4096;
static const UBaseType_t PRIORITY_BITMAP = 3;
static const BaseType_t CORE_BITMAP = 0;

#endif