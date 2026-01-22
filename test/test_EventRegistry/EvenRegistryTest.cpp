#include <unity.h>
#include <shared/EventTypes.h>
#include "include/EventRegistryTest.h"

void setUp(void)
{
    resetCallbackCounters();
}

void tearDown(void)
{
    clearEventRegistry();
}

#ifndef UNITY_NATIVE
void setup()
{
#else
int main(int argc, char **argv)
{
#endif
    UNITY_BEGIN();
    run_EventRegistry_tests();
    UNITY_END();
}

void loop()
{
    // Nothing to do here
}