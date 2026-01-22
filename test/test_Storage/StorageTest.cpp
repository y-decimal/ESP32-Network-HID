#include <unity.h>
#include "include/TestGenericStorage.h"
#include "include/TestTGenericStorage.h"

#ifndef UNITY_NATIVE
#include <Arduino.h>
#include <submodules/Storage/PreferencesStorage.h>
#else
#include "../FakeStorage.h"
#endif

#ifndef UNITY_NATIVE
PreferencesStorage testStorageImplementation("StorageTest");
#else
FakeStorage testStorageImplementation;
#endif

IStorage &testStorage = testStorageImplementation;

void setUp()
{
    // No setup needed with FakeStorage
}

void tearDown()
{
    testStorage.remove(TEST_KEY);
    testStorage.remove(LOAD_KEY);
    testStorage.remove(NONEXISTENT_KEY);
    testStorage.remove(MULTI_KEY);
}

#ifndef UNITY_NATIVE
void setup()
{
    delay(1000); // Wait for Preferences to be ready
#else
int main(int argc, char **argv)
{
#endif
    UNITY_BEGIN();
    run_ThreadSafeGenericStorage_tests();
    run_GenericStorage_tests();
    UNITY_END();
}

void loop()
{
    // No loop needed
}
