#ifndef LOGGER_H
#define LOGGER_H

#include <interfaces/ILogSink.h>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <functional>
#include <cstdarg>

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

    // Static global logging configuration
    static void setGlobalSink(ILogSink *globalSink);
    static void setNamespaceLevel(const char *logNamespace, LogLevel level);
    static LogLevel getNamespaceLevel(const char *logNamespace);
    static void setDefaultLogLevel(LogLevel level);
    static void setLogCallback(globalLogCallback callback);

    // Instance methods
    void setMode(LogMode mode);

    void error(const char *format, ...);
    void warn(const char *format, ...);
    void info(const char *format, ...);
    void debug(const char *format, ...);

    void log(const char *logNamespace, LogLevel level, const char *format, ...);

private:
    LogMode mode = LogMode::Local;
    std::string logNamespace;

    void logV(const char *logNamespace, LogLevel level, const char *format, va_list args);

    static void writeWithNamespace(const char *logNamespace, LogLevel level, const char *format, ...);
    static void writeWithNamespaceV(const char *logNamespace, LogLevel level, const char *format, va_list args);
};

#endif