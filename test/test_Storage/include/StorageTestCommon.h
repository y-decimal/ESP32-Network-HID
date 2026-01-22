#ifndef STORAGE_TEST_COMMON_H
#define STORAGE_TEST_COMMON_H

#include <interfaces/IStorage.h>

// Shared test constants
inline constexpr const char* TEST_KEY = "test_key";
inline constexpr const char* LOAD_KEY = "load_key";
inline constexpr const char* NONEXISTENT_KEY = "nonexistent_key";
inline constexpr const char* MULTI_KEY = "multi_key";

// Shared test storage reference (must be defined in main test file)
extern IStorage &testStorage;

#endif
