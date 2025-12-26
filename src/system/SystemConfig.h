#pragma once

// KeyScanner Task Config
static const uint32_t STACK_KEYSCAN = 4096;
static const UBaseType_t PRIORITY_KEYSCAN = 5;
static const BaseType_t CORE_KEYSCAN = 1;
static const TickType_t PERIOD_KEYSCAN_MS = 1;

// KeyScanner Pin Definitions
static const uint8_t ROWPINS[2] = {9, 10};
static const uint8_t COLPINS[2] = {17, 18};