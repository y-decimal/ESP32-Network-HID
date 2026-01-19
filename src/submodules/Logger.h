#ifndef LOGGER_H
#define LOGGER_H

#include <interfaces/ILogSink.h>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <functional>

class Logger
{
public:
    enum class LogMode : uint8_t
    {
        Local,
        Global
    };

    enum class LogLevel : uint8_t
    {
        disabled,
        error,
        warn,
        info,
        debug
    };

    using globalLogCallback = std::function<void(const char *logNamespace, LogLevel level, const char *message)>;

    Logger(const char *logNamespace);

    static void setGlobalSink(ILogSink *globalSink);
    static void setNamespaceLevel(const char *logNamespace, LogLevel level);
    static LogLevel getNamespaceLevel(const char *logNamespace);
    static void setDefaultLogLevel(LogLevel level);
    static void setLogCallback(globalLogCallback callback);
    static void writeWithNamespace(const char *logNamespace, LogLevel level, const char *message);

    void setMode(LogMode mode);

    void error(const char *message);
    void warn(const char *message);
    void info(const char *message);
    void debug(const char *message);

private:
    LogMode mode = LogMode::Local;
    std::string logNamespace;

    void log(const char *logNamespace, LogLevel level, const char *message);
};

#endif