#include <submodules/Logger.h>
#include <mutex>
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

void Logger::writeWithNamespace(const char *logNamespace, LogLevel level, const char *message)
{
    std::lock_guard<std::mutex> lock(LoggerCore::mutex);
    if (LoggerCore::globalSink == nullptr)
        return;

    // Create buffer on stack to avoid dangling pointer issues
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s : %s", logLevelToString(level), message);

    LoggerCore::globalSink->writeLog(logNamespace, buffer);
}

void Logger::setMode(LogMode mode)
{
    this->mode = mode;
}

void Logger::error(const char *message)
{
    const char *logNs = this->logNamespace.c_str();

    if (getNamespaceLevel(logNs) < LogLevel::error)
        return;

    log(logNs, LogLevel::error, message);
}

void Logger::warn(const char *message)
{
    const char *logNs = this->logNamespace.c_str();

    if (getNamespaceLevel(logNs) < LogLevel::warn)
        return;

    log(logNs, LogLevel::warn, message);
}

void Logger::info(const char *message)
{
    const char *logNs = this->logNamespace.c_str();

    if (getNamespaceLevel(logNs) < LogLevel::info)
        return;

    log(logNs, LogLevel::info, message);
}

void Logger::debug(const char *message)
{
    const char *logNs = this->logNamespace.c_str();

    if (getNamespaceLevel(logNs) < LogLevel::debug)
        return;

    log(logNs, LogLevel::debug, message);
}

void Logger::log(const char *logNamespace, LogLevel level, const char *message)
{
    switch (mode)
    {
    case LogMode::Local:
        writeWithNamespace(logNamespace, level, message);
        break;
    case LogMode::Global:
        if (LoggerCore::globalCallback)
            LoggerCore::globalCallback(logNamespace, level, message);
        break;
    }
}