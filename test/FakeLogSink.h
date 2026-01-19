#ifndef FAKELOGSINK_H
#define FAKELOGSINK_H

#include <interfaces/ILogSink.h>
#include <cstring>
#include <cstdio>

class FakeLogSink : public ILogSink
{
public:
    void writeLog(const char *logNamespace, const char *message) override
    {
        // Store the last log message for verification
        if (lastLogMessage) {
            delete[] lastLogMessage;
        }
        size_t len = strlen(logNamespace) + strlen(message) + 5; // for brackets, space, and null terminator (plus safety)
        lastLogMessage = new char[len];
        snprintf(lastLogMessage, len, "[%s] %s", logNamespace, message);
    }

    void clearLog() {
        if (lastLogMessage) {
            delete[] lastLogMessage;
            lastLogMessage = nullptr;
        }
    }

    const char* getLastLog() const {
        return lastLogMessage;
    }

    bool verifyLastLog(const char* expected) const {
        if (!lastLogMessage && !expected) {
            return true; // both are null
        }
        if (!lastLogMessage || !expected) {
            return false; // one is null, the other is not
        }
        return strcmp(lastLogMessage, expected) == 0;
    }
    
private: 
    char* lastLogMessage = nullptr;
};


#endif