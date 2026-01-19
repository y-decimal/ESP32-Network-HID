#include <submodules/Logger.h>
#include <mutex>
#include <cstdio>
namespace
{
    struct LoggerCore
    {
        static std::mutex mutex;
        static ILogSink *globalSink;
        static std::unordered_map<std::string, Logger::LogLevel> namespaceLevels;
        static Logger::globalLogCallback globalCallback;
        static Logger::LogLevel defaultLogLevel;
    };

    std::mutex LoggerCore::mutex{};
    ILogSink *LoggerCore::globalSink = nullptr;
    std::unordered_map<std::string, Logger::LogLevel> LoggerCore::namespaceLevels{};
    Logger::globalLogCallback LoggerCore::globalCallback = nullptr;
    Logger::LogLevel LoggerCore::defaultLogLevel = Logger::LogLevel::info;
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
    char messageBuffer[256];
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

void Logger::error(const char *format, ...)
{
    const char *logNs = this->logNamespace.c_str();

    if (getNamespaceLevel(logNs) < LogLevel::error)
        return;

    va_list args;
    va_start(args, format);
    log(logNs, LogLevel::error, format, args);
    va_end(args);
}

void Logger::warn(const char *format, ...)
{
    const char *logNs = this->logNamespace.c_str();

    if (getNamespaceLevel(logNs) < LogLevel::warn)
        return;

    va_list args;
    va_start(args, format);
    log(logNs, LogLevel::warn, format, args);
    va_end(args);
}

void Logger::info(const char *format, ...)
{
    const char *logNs = this->logNamespace.c_str();

    if (getNamespaceLevel(logNs) < LogLevel::info)
        return;

    va_list args;
    va_start(args, format);
    log(logNs, LogLevel::info, format, args);
    va_end(args);
}

void Logger::debug(const char *format, ...)
{
    const char *logNs = this->logNamespace.c_str();

    if (getNamespaceLevel(logNs) < LogLevel::debug)
        return;

    va_list args;
    va_start(args, format);
    log(logNs, LogLevel::debug, format, args);
    va_end(args);
}

void Logger::log(const char *logNamespace, LogLevel level, const char *format, va_list args)
{
    switch (mode)
    {
    case LogMode::Local:
        writeWithNamespaceV(logNamespace, level, format, args);
        break;
    case LogMode::Global:
        // Format the message for global callback
        char messageBuffer[256];
        vsnprintf(messageBuffer, sizeof(messageBuffer), format, args);
        std::lock_guard<std::mutex> lock(LoggerCore::mutex);
        if (LoggerCore::globalCallback)
            LoggerCore::globalCallback(logNamespace, level, messageBuffer);
        break;
    }
}