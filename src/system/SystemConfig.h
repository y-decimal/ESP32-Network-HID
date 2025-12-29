#ifndef SYSTEMCONFIG_H
#define SYSTEMCONFIG_H

#include <FreeRTOS.h>
#include <stdint.h>
#include <task.h>

// Global constants
inline constexpr uint8_t BITMAPSIZE =
    16; // 16 Bytes for 127 bits, should be enough for every possible key

// Priority Event Task Config
inline constexpr uint32_t STACK_PRIORITYEVENT = 4096;
inline constexpr UBaseType_t PRIORITY_PRIORITYEVENT = 4;
inline constexpr BaseType_t CORE_PRIORITYEVENT = 1;

// KeyScanner Task Config
inline constexpr uint32_t STACK_KEYSCAN = 4096;
inline constexpr UBaseType_t PRIORITY_KEYSCAN = 5;
inline constexpr BaseType_t CORE_KEYSCAN = 1;

// BitMapSender Task Config
inline constexpr uint32_t STACK_BITMAP = 4096;
inline constexpr UBaseType_t PRIORITY_BITMAP = 3;
inline constexpr BaseType_t CORE_BITMAP = 0;

#endif