#include <submodules/Logger.h>
#include <mutex>
#include <cstdio>
#include <cstring>

static constexpr size_t MAX_EARLY_LOG_MESSAGES = 32;
static constexpr size_t MAX_EARLY_LOG_MESSAGE_SIZE = 128;

namespace
{
    struct EarlyLogMessage
    {
        const char *logNamespace;
        Logger::LogLevel level;
        char message[MAX_EARLY_LOG_MESSAGE_SIZE];
    };

    struct LoggerCore
    {
        static std::mutex mutex;
        static ILogSink *globalSink;
        static std::unordered_map<std::string, Logger::LogLevel> namespaceLevels;
        static Logger::globalLogCallback globalCallback;
        static Logger::LogLevel defaultLogLevel;
        static EarlyLogMessage earlyMessages[MAX_EARLY_LOG_MESSAGES];
        static size_t earlyMessageCount;
        static size_t earlyMessageIndex;
        static bool loggingReady;
    };

    std::mutex LoggerCore::mutex{};
    ILogSink *LoggerCore::globalSink = nullptr;
    std::unordered_map<std::string, Logger::LogLevel> LoggerCore::namespaceLevels{};
    Logger::globalLogCallback LoggerCore::globalCallback = nullptr;
    Logger::LogLevel LoggerCore::defaultLogLevel = Logger::LogLevel::info;
    EarlyLogMessage LoggerCore::earlyMessages[MAX_EARLY_LOG_MESSAGES] = {};
    size_t LoggerCore::earlyMessageCount = 0;
    size_t LoggerCore::earlyMessageIndex = 0;
    bool LoggerCore::loggingReady = false;
}

static const char *logLevelToString(Logger::LogLevel level)
{
    switch (level)
    {
    case Logger::LogLevel::disabled:
        return "DISABLED";
    case Logger::LogLevel::error:
        return "ERROR";
    case Logger::LogLevel::warn:
        return "WARN";
    case Logger::LogLevel::info:
        return "INFO";
    case Logger::LogLevel::debug:
        return "DEBUG";
    default:
        return "UNKNOWN";
    }
}

Logger::Logger(const char *logNamespace)
{
    this->logNamespace.assign(logNamespace);
}

void Logger::setGlobalSink(ILogSink *globalSink)
{
    std::lock_guard<std::mutex> lock(LoggerCore::mutex);
    LoggerCore::globalSink = globalSink;
    LoggerCore::loggingReady = true;
    flushEarlyLogMessages();
}

void Logger::setNamespaceLevel(const char *logNamespace, LogLevel level)
{
    std::lock_guard<std::mutex> lock(LoggerCore::mutex);
    LoggerCore::namespaceLevels[logNamespace] = level;
}

Logger::LogLevel Logger::getNamespaceLevel(const char *logNamespace)
{
    std::lock_guard<std::mutex> lock(LoggerCore::mutex);
    auto it = LoggerCore::namespaceLevels.find(logNamespace);
    if (it == LoggerCore::namespaceLevels.end())
        return LoggerCore::defaultLogLevel;
    return it->second;
}

void Logger::setDefaultLogLevel(LogLevel level)
{
    std::lock_guard<std::mutex> lock(LoggerCore::mutex);
    LoggerCore::defaultLogLevel = level;
}

void Logger::setLogCallback(globalLogCallback callback)
{
    std::lock_guard<std::mutex> lock(LoggerCore::mutex);
    LoggerCore::globalCallback = std::move(callback);
}

void Logger::writeWithNamespaceV(const char *logNamespace, LogLevel level, const char *format, va_list args)
{
    std::lock_guard<std::mutex> lock(LoggerCore::mutex);
    if (LoggerCore::globalSink == nullptr)
        return;

    // Format the message first
    char messageBuffer[MAX_EARLY_LOG_MESSAGE_SIZE];
    vsnprintf(messageBuffer, sizeof(messageBuffer), format, args);

    // Create final buffer with log level prefix
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "%s : %s", logLevelToString(level), messageBuffer);

    LoggerCore::globalSink->writeLog(logNamespace, buffer);
}

void Logger::writeWithNamespace(const char *logNamespace, LogLevel level, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    writeWithNamespaceV(logNamespace, level, format, args);
    va_end(args);
}

void Logger::setMode(LogMode mode)
{
    this->mode = mode;
}

void Logger::logV(const char *logNamespace, LogLevel level, const char *format, va_list args)
{
    switch (mode)
    {
    case LogMode::Local:
        writeWithNamespaceV(logNamespace, level, format, args);
        break;
    case LogMode::Global:
        // Format the message for global callback
        char messageBuffer[MAX_EARLY_LOG_MESSAGE_SIZE];
        vsnprintf(messageBuffer, sizeof(messageBuffer), format, args);
        std::lock_guard<std::mutex> lock(LoggerCore::mutex);
        if (LoggerCore::globalCallback)
            LoggerCore::globalCallback(logNamespace, level, messageBuffer);
        break;
    }
}

void Logger::error(const char *format, ...)
{
    const char *logNs = this->logNamespace.c_str();

    if (getNamespaceLevel(logNs) < LogLevel::error)
        return;

    va_list args;
    va_start(args, format);
    logV(logNs, LogLevel::error, format, args);
    va_end(args);
}

void Logger::warn(const char *format, ...)
{
    const char *logNs = this->logNamespace.c_str();

    if (getNamespaceLevel(logNs) < LogLevel::warn)
        return;

    va_list args;
    va_start(args, format);
    logV(logNs, LogLevel::warn, format, args);
    va_end(args);
}

void Logger::info(const char *format, ...)
{
    const char *logNs = this->logNamespace.c_str();

    if (getNamespaceLevel(logNs) < LogLevel::info)
        return;

    va_list args;
    va_start(args, format);
    logV(logNs, LogLevel::info, format, args);
    va_end(args);
}

void Logger::debug(const char *format, ...)
{
    const char *logNs = this->logNamespace.c_str();

    if (getNamespaceLevel(logNs) < LogLevel::debug)
        return;

    va_list args;
    va_start(args, format);
    logV(logNs, LogLevel::debug, format, args);
    va_end(args);
}

void Logger::log(const char *logNamespace, LogLevel level, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logV(logNamespace, level, format, args);
    va_end(args);
}

void Logger::storeEarlyLogMessage(const char *logNamespace, LogLevel level, const char *format, va_list args)
{
    std::lock_guard<std::mutex> lock(LoggerCore::mutex);

    if (LoggerCore::earlyMessageCount >= MAX_EARLY_LOG_MESSAGES)
        return;

    char messageBuffer[MAX_EARLY_LOG_MESSAGE_SIZE];
    vsnprintf(messageBuffer, sizeof(messageBuffer), format, args);

    if (LoggerCore::globalCallback)
    {
        LoggerCore::globalCallback(logNamespace, level, messageBuffer);
        return;
    }
    size_t currentIndex = LoggerCore::earlyMessageIndex;
    LoggerCore::earlyMessageIndex += MAX_EARLY_LOG_MESSAGE_SIZE;
    memcpy(LoggerCore::earlyMessages + currentIndex, messageBuffer, MAX_EARLY_LOG_MESSAGE_SIZE);
    LoggerCore::earlyMessageCount++;
}

void Logger::flushEarlyLogMessages()
{
    std::lock_guard<std::mutex> lock(LoggerCore::mutex);

    for (size_t i = 0; i < LoggerCore::earlyMessageCount; i++)
    {
        EarlyLogMessage msg = LoggerCore::earlyMessages[i];
        writeWithNamespace(msg.logNamespace, msg.level, msg.message);
        memset(&LoggerCore::earlyMessages[i], 0, sizeof(EarlyLogMessage));
    }
    LoggerCore::earlyMessageIndex = 0;
    LoggerCore::earlyMessageCount = 0;
}