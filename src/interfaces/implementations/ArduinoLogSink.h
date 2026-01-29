#ifndef ARDUINOLOGSINK_H
#define ARDUINOLOGSINK_H

#include <interfaces/ILogSink.h>
#include <Arduino.h>

class ArduinoLogSink : public ILogSink
{
public:
    void writeLog(const char *logNamespace, const char *message) override
    {
        printf("[%s] %s\n", logNamespace, message);
    }
};

#endif