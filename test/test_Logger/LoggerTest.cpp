#include "../FakeLogSink.h"
#include "include/LoggerTest.h"
#include <unity.h>

void setUp()
{
    // No setup needed
}

void tearDown()
{
    // No teardown needed
}



#ifndef UNITY_NATIVE
void setup()
{
#else
int main(int argc, char **argv)
{
#endif
    UNITY_BEGIN();
    run_Logger_tests();
    UNITY_END();
}

void loop()
{
    // No loop needed
}