#include "../FakeLogSink.h"
#include <submodules/Logger.h>
#include <submodules/Logger.cpp>
#include <unity.h>

void setUp()
{
    // No setup needed
}

void tearDown()
{
    // No teardown needed
}

void test_Logger_localLogging()
{
    FakeLogSink fakeSink;
    Logger::setGlobalSink(&fakeSink);
    Logger::setNamespaceLevel("TestNamespace", Logger::LogLevel::debug);

    Logger logger("TestNamespace");
    logger.setMode(Logger::LogMode::Local);

    logger.debug("Debug message");
    TEST_ASSERT_TRUE(fakeSink.verifyLastLog("[TestNamespace] DEBUG : Debug message"));

    logger.info("Info message");
    TEST_ASSERT_TRUE(fakeSink.verifyLastLog("[TestNamespace] INFO : Info message"));

    logger.warn("Warn message");
    TEST_ASSERT_TRUE(fakeSink.verifyLastLog("[TestNamespace] WARN : Warn message"));

    logger.error("Error message");
    TEST_ASSERT_TRUE(fakeSink.verifyLastLog("[TestNamespace] ERROR : Error message"));

    fakeSink.clearLog();
}

void test_Logger_globalLoggingNoCallback()
{
    FakeLogSink fakeSink;
    Logger::setGlobalSink(&fakeSink);
    Logger::setNamespaceLevel("GlobalNamespace", Logger::LogLevel::debug);

    Logger logger("GlobalNamespace");
    logger.setMode(Logger::LogMode::Global);

    logger.debug("Global Debug message");
    TEST_ASSERT_TRUE(fakeSink.verifyLastLog("[GlobalNamespace (LOCAL)] DEBUG : Global Debug message")); // Log falls back to local mode because no callback is set

    logger.info("Global Info message");
    TEST_ASSERT_TRUE(fakeSink.verifyLastLog("[GlobalNamespace (LOCAL)] INFO : Global Info message")); // Log falls back to local mode because no callback is set

    logger.warn("Global Warn message");
    TEST_ASSERT_TRUE(fakeSink.verifyLastLog("[GlobalNamespace (LOCAL)] WARN : Global Warn message")); // Log falls back to local mode because no callback is set

    logger.error("Global Error message");
    TEST_ASSERT_TRUE(fakeSink.verifyLastLog("[GlobalNamespace (LOCAL)] ERROR : Global Error message")); // Log falls back to local mode because no callback is set

    fakeSink.clearLog();
}

void test_Logger_globalLoggingWithCallback()
{
    FakeLogSink fakeSink;
    Logger::setGlobalSink(&fakeSink);
    Logger::setNamespaceLevel("GlobalNamespace", Logger::LogLevel::debug);

    bool callbackInvoked = false;

    Logger::setLogCallback([&](const char *logNamespace, Logger::LogLevel level, const char *message) {
        callbackInvoked = true;
        fakeSink.writeLog(logNamespace, message);
    });

    Logger logger("GlobalNamespace");
    logger.setMode(Logger::LogMode::Global);

    logger.debug("Global Debug message");
    TEST_ASSERT_TRUE(callbackInvoked);
    TEST_ASSERT_TRUE(fakeSink.verifyLastLog("[GlobalNamespace] Global Debug message"));
    callbackInvoked = false;
    fakeSink.clearLog();

    logger.info("Global Info message");
    TEST_ASSERT_TRUE(callbackInvoked);
    TEST_ASSERT_TRUE(fakeSink.verifyLastLog("[GlobalNamespace] Global Info message"));
    callbackInvoked = false;
    fakeSink.clearLog();

    logger.warn("Global Warn message");
    TEST_ASSERT_TRUE(callbackInvoked);
    TEST_ASSERT_TRUE(fakeSink.verifyLastLog("[GlobalNamespace] Global Warn message"));
    callbackInvoked = false;
    fakeSink.clearLog();

    logger.error("Global Error message");
    TEST_ASSERT_TRUE(callbackInvoked);
    TEST_ASSERT_TRUE(fakeSink.verifyLastLog("[GlobalNamespace] Global Error message"));
    fakeSink.clearLog();
}

#ifndef UNITY_NATIVE
void setup()
{
#else
int main(int argc, char **argv)
{
#endif
    UNITY_BEGIN();
    RUN_TEST(test_Logger_localLogging);
    RUN_TEST(test_Logger_globalLoggingNoCallback);
    RUN_TEST(test_Logger_globalLoggingWithCallback);
    UNITY_END();
}

void loop()
{
    // No loop needed
}