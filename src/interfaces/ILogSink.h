#ifndef ILOGSINK_H
#define ILOGSINK_H

class ILogSink
{
public:
    virtual void writeLog(const char *logNamespace, const char *message) = 0;
    virtual ~ILogSink() = default;
};

#endif