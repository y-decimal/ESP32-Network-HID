#ifndef SYSTEMCONFIG_H
#define SYSTEMCONFIG_H

#include <FreeRTOS.h>
#include <stdint.h>
#include <task.h>

// Task Manager Task Config
static constexpr uint32_t STACK_TASKMANAGER = 4096;
static constexpr UBaseType_t PRIORITY_TASKMANAGER = 5;
static constexpr BaseType_t CORE_TASKMANAGER = 0;
static constexpr TickType_t PERIOD_TASKMANAGER = pdMS_TO_TICKS(500);

// Priority Event Task Config
static constexpr uint32_t STACK_EVENTBUS = 4096;
static constexpr UBaseType_t PRIORITY_EVENTBUS = 5;
static constexpr BaseType_t CORE_EVENTBUS = 1;

// KeyScanner Task Config
static constexpr uint32_t STACK_KEYSCAN = 4096;
static constexpr UBaseType_t PRIORITY_KEYSCAN = 5;
static constexpr BaseType_t CORE_KEYSCAN = 1;

// Slave ESP Task Config
static constexpr uint32_t STACK_SLAVEESP = 4096;
static constexpr UBaseType_t PRIORITY_SLAVEESP = 5;
static constexpr BaseType_t CORE_SLAVEESP = 0;

// Master ESP Task Config
static constexpr uint32_t STACK_MASTERESP = 4096;
static constexpr UBaseType_t PRIORITY_MASTERESP = 5;
static constexpr BaseType_t CORE_MASTERESP = 0;

// Logger Task Config
static constexpr uint32_t STACK_LOGGER = 4096;
static constexpr UBaseType_t PRIORITY_LOGGER = 3;
static constexpr BaseType_t CORE_LOGGER = 0;

#endif