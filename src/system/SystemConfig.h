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
static constexpr uint32_t STACK_PRIORITYEVENT = 4096;
static constexpr UBaseType_t PRIORITY_PRIORITYEVENT = 5;
static constexpr BaseType_t CORE_PRIORITYEVENT = 1;

// Event Task Config
static constexpr uint32_t STACK_EVENT = 4096;
static constexpr UBaseType_t PRIORITY_EVENT = 1;
static constexpr BaseType_t CORE_EVENT = 0;

// KeyScanner Task Config
static constexpr uint32_t STACK_KEYSCAN = 4096;
static constexpr UBaseType_t PRIORITY_KEYSCAN = 5;
static constexpr BaseType_t CORE_KEYSCAN = 1;

#endif