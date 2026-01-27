#ifndef SYSTEMCONFIG_H
#define SYSTEMCONFIG_H

#include <FreeRTOS.h>
#include <stdint.h>
#include <task.h>

// Event Bus Task Config
static constexpr uint32_t STACK_EVENTBUS = 4096;
static constexpr UBaseType_t PRIORITY_EVENTBUS = 5;
static constexpr BaseType_t CORE_EVENTBUS = 1;

// KeyScanner Task Config
static constexpr uint32_t STACK_KEYSCAN = 4096;
static constexpr UBaseType_t PRIORITY_KEYSCAN = 5;
static constexpr BaseType_t CORE_KEYSCAN = 1;

// Slave Task Config
static constexpr uint32_t STACK_SLAVE = 4096;
static constexpr UBaseType_t PRIORITY_SLAVE = 5;
static constexpr BaseType_t CORE_SLAVE = 0;

// Master Task Config
static constexpr uint32_t STACK_MASTER = 4096;
static constexpr UBaseType_t PRIORITY_MASTER = 5;
static constexpr BaseType_t CORE_MASTER = 0;

// Logger Task Config
static constexpr uint32_t STACK_LOGGER = 4096;
static constexpr UBaseType_t PRIORITY_LOGGER = 3;
static constexpr BaseType_t CORE_LOGGER = 0;

// Hid Output Task Config
static constexpr uint32_t STACK_HIDOUTPUT = 4096;
static constexpr UBaseType_t PRIORITY_HIDOUTPUT = 5;
static constexpr BaseType_t CORE_HIDOUTPUT = 1;

#endif