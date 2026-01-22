#include <unity.h>
#include "include/ConfigManagerTest.h"

#ifndef UNITY_NATIVE
#include <Arduino.h>
#include <submodules/Storage/PreferencesStorage.h>
#else
#include "../FakeStorage.h"
#endif

#ifndef UNITY_NATIVE
PreferencesStorage testStorageImplementation("CfgMgrTest");
#else
FakeStorage testStorageImplementation;
#endif

IStorage &testStorage = testStorageImplementation;

void setUp() {
  // No setup needed with FakeStorage
}

void tearDown() {
  testStorage.remove(CONFIG_MANAGER_NAMESPACE "/" GLOBAL_CONFIG_KEY);
  testStorage.remove(CONFIG_MANAGER_NAMESPACE "/" KEYSCANNER_CONFIG_KEY);
}

#ifndef UNITY_NATIVE
void setup() {
  delay(1000); // Wait for Preferences to be ready
#else
int main(int argc, char **argv) {
#endif
  UNITY_BEGIN();
  run_ConfigManager_tests();
  UNITY_END();
}

void loop() {
  // No loop needed
}