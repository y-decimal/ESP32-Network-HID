#include <submodules/Logger.h>
#include <mutex>
#include <cstdio>
#include <cstring>

namespace
{
    struct EarlyLogMessage
    {
        char logNamespace[Logger::MAX_NAMESPACE_LENGTH];
        Logger::LogLevel level;
        char message[Logger::MAX_EARLY_LOG_MESSAGE_SIZE];
    };

    struct LoggerCore
    {
        static std::mutex mutex;
        static ILogSink *globalSink;
        static std::unordered_map<std::string, Logger::LogLevel> namespaceLevels;
        static Logger::globalLogCallback globalCallback;
        static Logger::LogLevel defaultLogLevel;
        static EarlyLogMessage earlyMessages[Logger::MAX_EARLY_LOG_MESSAGES];
        static size_t earlyMessageCount;
        static bool loggingReady;
    };

    std::mutex LoggerCore::mutex{};
    ILogSink *LoggerCore::globalSink = nullptr;
    std::unordered_map<std::string, Logger::LogLevel> LoggerCore::namespaceLevels{};
    Logger::globalLogCallback LoggerCore::globalCallback = nullptr;
    Logger::LogLevel LoggerCore::defaultLogLevel = Logger::LogLevel::info;
    EarlyLogMessage LoggerCore::earlyMessages[Logger::MAX_EARLY_LOG_MESSAGES] = {};
    size_t LoggerCore::earlyMessageCount = 0;
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
    strncpy(this->logNamespace, logNamespace, MAX_NAMESPACE_LENGTH - 1);
    this->logNamespace[sizeof(this->logNamespace) - 1] = '\0';
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

void Logger::setMode(LogMode mode)
{
    this->mode = mode;
}

void Logger::error(const char *format, ...)
{
    const char *logNs = this->logNamespace;

    if (getNamespaceLevel(logNs) < LogLevel::error)
        return;

    va_list args;
    va_start(args, format);
    logV(logNs, LogLevel::error, format, args);
    va_end(args);
}

void Logger::warn(const char *format, ...)
{
    const char *logNs = this->logNamespace;

    if (getNamespaceLevel(logNs) < LogLevel::warn)
        return;

    va_list args;
    va_start(args, format);
    logV(logNs, LogLevel::warn, format, args);
    va_end(args);
}

void Logger::info(const char *format, ...)
{
    const char *logNs = this->logNamespace;

    if (getNamespaceLevel(logNs) < LogLevel::info)
        return;

    va_list args;
    va_start(args, format);
    logV(logNs, LogLevel::info, format, args);
    va_end(args);
}

void Logger::debug(const char *format, ...)
{
    const char *logNs = this->logNamespace;

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

void Logger::logV(const char *logNamespace, LogLevel level, const char *format, va_list args)
{
    char msg[MAX_EARLY_LOG_MESSAGE_SIZE];
    vsnprintf(msg, sizeof(msg), format, args);

    switch (mode)
    {
    case LogMode::Local:
        LoggerCore::loggingReady
            ? internalWrite(logNamespace, level, msg)         // Write directly
            : storeEarlyLogMessage(logNamespace, level, msg); // Store early message
        return;
    case LogMode::Global:
        if (LoggerCore::globalCallback)
            LoggerCore::globalCallback(logNamespace, level, msg);
        else
            LoggerCore::loggingReady
                ? internalWrite(logNamespace, level, msg)                               // Write directly       
                : storeEarlyLogMessage(getModifiedNamespace(logNamespace), level, msg); // Store early message
        return;
    }
}

void Logger::internalWrite(const char *logNamespace, Logger::LogLevel level, const char *msg)
{
    // Create buffer with log level prefix
    constexpr size_t BUFFER_SIZE = MAX_EARLY_LOG_MESSAGE_SIZE + MAX_NAMESPACE_LENGTH + sizeof(" : ") + 10;
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%s : %s", logLevelToString(level), msg);

    LoggerCore::globalSink->writeLog(logNamespace, buffer);
}

void Logger::storeEarlyLogMessage(const char *logNamespace, LogLevel level, const char *msg)
{
    if (LoggerCore::earlyMessageCount >= MAX_EARLY_LOG_MESSAGES)
        return;

    EarlyLogMessage earlyMsg;
    strncpy(earlyMsg.logNamespace, logNamespace, MAX_NAMESPACE_LENGTH - 1);
    earlyMsg.logNamespace[MAX_NAMESPACE_LENGTH - 1] = '\0';
    earlyMsg.level = level;
    strncpy(earlyMsg.message, msg, MAX_EARLY_LOG_MESSAGE_SIZE - 1);
    earlyMsg.message[MAX_EARLY_LOG_MESSAGE_SIZE - 1] = '\0';
    LoggerCore::earlyMessages[LoggerCore::earlyMessageCount] = earlyMsg;

    LoggerCore::earlyMessageCount++;
}

void Logger::flushEarlyLogMessages()
{
    internalWrite("LOG", LogLevel::info, "Flushing early log messages");
    for (size_t i = 0; i < LoggerCore::earlyMessageCount; i++)
    {
        EarlyLogMessage msg = LoggerCore::earlyMessages[i];
        internalWrite(msg.logNamespace, msg.level, msg.message);
    }
    clearEarlyLogMessages();
}

void Logger::clearEarlyLogMessages()
{
    for (size_t i = 0; i < LoggerCore::earlyMessageCount; i++)
    {
        LoggerCore::earlyMessages[i] = EarlyLogMessage{};
    }
    LoggerCore::earlyMessageCount = 0;
}

inline const char *Logger::getModifiedNamespace(const char *originalNamespace)
{
    static char modifiedNamespace[MAX_NAMESPACE_LENGTH];
    snprintf(modifiedNamespace, sizeof(modifiedNamespace), "%s (LOCAL)", originalNamespace);
    return modifiedNamespace;
}